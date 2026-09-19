#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void babyCucco_state_uninitialized_hook(GB *gb);
void babyCucco_state_grabbed_hook(GB *gb);
void babyCucco_state_stub_hook(GB *gb);
void babyCucco_state8_hook(GB *gb);
void babyCucco_animate_hook(GB *gb);
void babyCucco_state9_hook(GB *gb);
void babyCucco_updateAnimationFromAngle_hook(GB *gb);

static uint16_t babyCucco_jump_table(GB *gb) {
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
// ENEMY_BABY_CUCCO
// ==================================================================================================
void enemyCode33_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x486e, 0x4870); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4870, 0x4871); A = mem_rd(gb, DE);
  {
    CYC(0x4871, 0x4872); push_effect(gb, 0x4872);
    uint16_t target = babyCucco_jump_table(gb);
    if (target == 0x4886) { babyCucco_state_uninitialized_hook(gb); return; }
    if (target == 0x48e4) { babyCucco_state_stub_hook(gb); return; }
    if (target == 0x488b) { babyCucco_state_grabbed_hook(gb); return; }
    if (target == 0x48e5) { babyCucco_state8_hook(gb); return; }
    if (target == 0x490f) { babyCucco_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

void babyCucco_state_uninitialized_hook(GB *gb) {
  CYC(0x4886, 0x4888); A = 0x0a; // SPEED_40
  CYC(0x4888, 0x488b); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

void babyCucco_state_grabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x488b, 0x488c); E = alu_inc8(gb, E);
  CYC(0x488c, 0x488d); A = mem_rd(gb, DE);
  {
    CYC(0x488d, 0x488e); push_effect(gb, 0x488e);
    uint16_t target = babyCucco_jump_table(gb);
    if (target == 0x4896) goto justGrabbed;
    if (target == 0x48b1) goto beingHeld;
    if (target == 0x48c2) goto released;
    if (target == 0x48d4) goto landed;
    HANDOFF(target);
  }

justGrabbed:
  CYC(0x4896, 0x4897); H = D;
  CYC(0x4897, 0x4898); L = E;
  CYC(0x4898, 0x4899); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x4899, 0x489b); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x489b, 0x489d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x489d, 0x489e); alu_xor(gb, A);
  CYC(0x489e, 0x48a1); mem_wr(gb, wLinkGrabState2, A);
  CYC(0x48a1, 0x48a4); A = mem_rd(gb, w1Link_direction);
  CYC(0x48a4, 0x48a6); A = alu_srl(gb, A);
  CYC(0x48a6, 0x48a8); alu_xor(gb, 0x01);
  CYC(0x48a8, 0x48aa); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x48aa, 0x48ab); mem_wr(gb, HL, A);
  CALL_C(0x48ab, enemySetAnimation_hook, 0x282b, 0x48ae);
  CYC(0x48ae, 0x48b1); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  CYC(0x48b1, 0x48b2); H = D;
  CYC(0x48b2, 0x48b4); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x48b4, 0x48b7); A = mem_rd(gb, w1Link_direction);
  CYC(0x48b7, 0x48b9); A = alu_srl(gb, A);
  CYC(0x48b9, 0x48bb); alu_xor(gb, 0x01);
  CYC(0x48bb, 0x48bc); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x48bc, 0x48be); goto released; } // jr z
  CYC(0x48bc, 0x48be);
  CYC(0x48be, 0x48bf); mem_wr(gb, HL, A);
  CYC(0x48bf, 0x48c2); enemySetAnimation_hook(gb); return; // jp

released:
  CYC(0x48c2, 0x48c4); E = ENEMY_BASE + OBJ_YH;
  CYC(0x48c4, 0x48c5); A = mem_rd(gb, DE);
  CYC(0x48c5, 0x48c7); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x48c7, 0x48c9); goto delete_; } // jr nc
  CYC(0x48c7, 0x48c9);
  CYC(0x48c9, 0x48cb); E = ENEMY_BASE + OBJ_XH;
  CYC(0x48cb, 0x48cc); A = mem_rd(gb, DE);
  CYC(0x48cc, 0x48ce); alu_cp(gb, 0xa0); // SMALL_ROOM_WIDTH<<4
  if (F & FC) { CYCT(0x48ce, 0x48d1); enemyAnimate_hook(gb); return; } // jp c
  CYC(0x48ce, 0x48d1);

delete_:
  CYC(0x48d1, 0x48d4); enemyDelete_hook(gb); return; // jp

landed:
  CYC(0x48d4, 0x48d5); H = D;
  CYC(0x48d5, 0x48d7); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x48d7, 0x48d9); mem_wr(gb, HL, 0x08);
  CYC(0x48d9, 0x48db); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x48db, 0x48dd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x48dd, 0x48df); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x48df, 0x48e1); mem_wr(gb, HL, 0xff);
  CYC(0x48e1, 0x48e4); objectSetVisiblec2_hook(gb); return; // jp
}

void babyCucco_state_stub_hook(GB *gb) {
  RET(0x48e4); return; // ret
}

void babyCucco_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x48e5, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x48e8);
  CALL_C(0x48e8, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x48eb);
  CALL_C(0x48eb, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x48ee);
  CALL_C(0x48ee, babyCucco_updateAnimationFromAngle_hook, 0x491a, 0x48f1);
  CYC(0x48f1, 0x48f3); C = 0x10;
  CALL_C(0x48f3, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x48f6);
  if (!(F & FC)) { CYCT(0x48f6, 0x48f8); goto moveCloserToLink; } // jr nc
  CYC(0x48f6, 0x48f8);
  CALL_C(0x48f8, getRandomNumber_noPreserveVars_hook, 0x0453, 0x48fb);
  CYC(0x48fb, 0x48fd); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(0x48fd); return; } // ret nz
  CYC(0x48fd, 0x48fe);
  CALL_C(0x48fe, ecom_incState_b0e_hook, 0x4000, 0x4901);
  CYC(0x4901, 0x4903); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x4903, 0x4905); A = 0x40;
  CYC(0x4905, 0x4906); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4906, 0x4908); mem_wr(gb, HL, 0xff);
  RET(0x4908); return; // ret

moveCloserToLink:
  CALL_C(0x4909, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x490c);
  babyCucco_animate_hook(gb); return; // fallthrough
}

void babyCucco_animate_hook(GB *gb) {
  CYC(0x490c, 0x490f); enemyAnimate_hook(gb); return; // jp
}

// Hopping
void babyCucco_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x490f, 0x4911); C = 0x12;
  CALL_C(0x4911, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4914);
  if (!(F & FZ)) { CYCT(0x4914, 0x4916); babyCucco_animate_hook(gb); return; } // jr nz
  CYC(0x4914, 0x4916);
  CYC(0x4916, 0x4918); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4918, 0x4919); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  RET(0x4919); return; // ret
}

void babyCucco_updateAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x491a, 0x491c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x491c, 0x491d); A = mem_rd(gb, DE);
  CYC(0x491d, 0x491f); alu_cp(gb, 0x10);
  CYC(0x491f, 0x4921); A = 0x01;
  if (F & FC) { CYCT(0x4921, 0x4923); goto L_4924; } // jr c
  CYC(0x4921, 0x4923);
  CYC(0x4923, 0x4924); alu_xor(gb, A);

L_4924:
  CYC(0x4924, 0x4925); H = D;
  CYC(0x4925, 0x4927); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x4927, 0x4928); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4928); return; } // ret z
  CYC(0x4928, 0x4929);
  CYC(0x4929, 0x492a); mem_wr(gb, HL, A);
  CYC(0x492a, 0x492d); enemySetAnimation_hook(gb); return; // jp
}
