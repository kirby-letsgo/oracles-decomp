#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode0b_jump_table(GB *gb) {
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

static void enemyCode0b_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 0d:48f3, called from enemyCode0b@state8 (subid00 & subid01).
void enemyCode0b_chooseSpawnPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x48f3, 0x48f4); A = B;
  CYC(0x48f4, 0x48f5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x48f5, 0x48f7); goto chooseRandomSpot; } // jr nz
  CYC(0x48f5, 0x48f7);
  CYC(0x48f7, 0x48fa); SET_DE(0xd00b); // w1Link.yh
  CALL_C(0x48fa, getShortPositionFromDE_hook, 0x209b, 0x48fd);
  CYC(0x48fd, 0x48fe); C = A;
  CYC(0x48fe, 0x4900); E = 0x08; // <w1Link.direction
  CYC(0x4900, 0x4901); A = mem_rd(gb, DE);
  CYC(0x4901, 0x4902); alu_rlca(gb);
  CYC(0x4902, 0x4903); alu_rlca(gb);
  CYC(0x4903, 0x4906); SET_HL(0x4927); // @@linkRelativeOffsets
  CYC(0x4906, 0x4907); enemyCode0b_addAToHl_from_rst(gb, 0x4907);
  CYC(0x4907, 0x490a); A = mem_rd(gb, wFrameCounter);
  CYC(0x490a, 0x490c); alu_and(gb, 0x03);
  CYC(0x490c, 0x490d); enemyCode0b_addAToHl_from_rst(gb, 0x490d);
  CYC(0x490d, 0x490f); A = H8(hActiveObject);
  CYC(0x490f, 0x4910); D = A;
  CYC(0x4910, 0x4911); A = C;
  CYC(0x4911, 0x4912); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4912, 0x4913); C = A;
  CYC(0x4913, 0x4915); alu_and(gb, 0xf0);
  CYC(0x4915, 0x4917); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x4917, 0x4919); goto invalid; } // jr nc
  CYC(0x4917, 0x4919);
  CYC(0x4919, 0x491a); A = C;
  CYC(0x491a, 0x491c); alu_and(gb, 0x0f);
  CYC(0x491c, 0x491e); alu_cp(gb, 0x0a); // SMALL_ROOM_WIDTH
  if (!(F & FC)) { CYCT(0x491e, 0x4920); goto invalid; } // jr nc
  CYC(0x491e, 0x4920);
  CYC(0x4920, 0x4922); B = 0xce; // >wRoomCollisions
  CYC(0x4922, 0x4923); A = mem_rd(gb, BC);
  CYC(0x4923, 0x4924); alu_or(gb, A);
  RET(0x4924); return; // ret

invalid:
  CYC(0x4925, 0x4926); alu_or(gb, D);
  RET(0x4926); return; // ret

chooseRandomSpot:
  CALL_C(0x4937, getRandomNumber_noPreserveVars_hook, 0x0453, 0x493a);
  CYC(0x493a, 0x493c); alu_and(gb, 0x77);
  CYC(0x493c, 0x493d); C = A;
  CYC(0x493d, 0x493f); B = 0xce; // >wRoomCollisions
  CYC(0x493f, 0x4940); A = mem_rd(gb, BC);
  CYC(0x4940, 0x4941); alu_or(gb, A);
  RET(0x4941); return; // ret
}

// 0d:4942, called from enemyCode0b's uninitialized/subid02 states.
void enemyCode0b_setRandomCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4942, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4945);
  CYC(0x4945, 0x4947); alu_and(gb, 0x03);
  CYC(0x4947, 0x494a); SET_HL(0x4950); // @counter1Vals
  CYC(0x494a, 0x494b); enemyCode0b_addAToHl_from_rst(gb, 0x494b);
  CYC(0x494b, 0x494d); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x494d, 0x494e); A = mem_rd(gb, HL);
  CYC(0x494e, 0x494f); mem_wr(gb, DE, A);
  RET(0x494f); return; // ret
}

// 0d:4954, called after emerging fully in the chase states.
void enemyCode0b_setRandomHighCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4954, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4957);
  CYC(0x4957, 0x4959); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4959, 0x495b); alu_and(gb, 0x38);
  CYC(0x495b, 0x495d); alu_add(gb, 0x70);
  CYC(0x495d, 0x495e); mem_wr(gb, DE, A);
  RET(0x495e); return; // ret
}

// 0d:495f, called from subid02's chase state.
void enemyCode0b_nudgeTowardsLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x495f, ecom_decCounter2_b0d_hook, 0x43a3, 0x4962);
  if (!(F & FZ)) { RET_TAKEN(0x4962); return; } // ret nz
  CYC(0x4962, 0x4963);
  CYC(0x4963, 0x4965); mem_wr(gb, HL, 0x06);
  CALL_C(0x4965, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4968);
  CYC(0x4968, 0x496b); objectNudgeAngleTowards_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_LEEVER
// ==================================================================================================
void enemyCode0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x478c, ecom_checkHazards_b0d_hook, 0x4051, 0x478f);
  if (F & FZ) { CYCT(0x478f, 0x4791); goto normalStatus; } // jr z
  CYC(0x478f, 0x4791);
  CYC(0x4791, 0x4793); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4793); return; } // ret c
  CYC(0x4793, 0x4794);
  if (F & FZ) { CYCT(0x4794, 0x4796); goto dead; } // jr z
  CYC(0x4794, 0x4796);
  CYC(0x4796, 0x4797); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4797, 0x479a); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4797, 0x479a);
  RET(0x479a); return; // ret

dead:
  CYC(0x479b, 0x479d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x479d, 0x479e); A = mem_rd(gb, DE);
  CYC(0x479e, 0x47a0); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x47a0, 0x47a2); goto die; } // jr nz
  CYC(0x47a0, 0x47a2);
  CYC(0x47a2, 0x47a4); B = 0x0b; // ENEMY_LEEVER
  CALL_C(0x47a4, ecom_spawnEnemyWithSubid01_b0d_hook, 0x4373, 0x47a7);
  if (!(F & FZ)) { RET_TAKEN(0x47a7); return; } // ret nz
  CYC(0x47a7, 0x47a8);
  CYC(0x47a8, 0x47a9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x47a9, 0x47ab); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x47ab, 0x47ad); L = ENEMY_BASE + OBJ_YH;
  CYC(0x47ad, 0x47ae); A = mem_rd(gb, DE);
  CYC(0x47ae, 0x47af); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x47af, 0x47b0); E = alu_inc8(gb, E);
  CYC(0x47b0, 0x47b1); L = alu_inc8(gb, L);
  CYC(0x47b1, 0x47b2); A = mem_rd(gb, DE);
  CYC(0x47b2, 0x47b3); mem_wr(gb, HL, A);

die:
  CYC(0x47b3, 0x47b6); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x47b6, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x47b9);
  if (!(F & FC)) { CYCT(0x47b9, 0x47bb); goto normalState; } // jr nc
  CYC(0x47b9, 0x47bb);
  {
    CYC(0x47bb, 0x47bc); push_effect(gb, 0x47bc);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x47d4) goto state_uninitialized;
    if (target == 0x47f4) goto state_stub;
    if (target == 0x47da) goto state_switchHook;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x47cc, 0x47cd); A = B;
  {
    CYC(0x47cd, 0x47ce); push_effect(gb, 0x47ce);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x47f5) goto normalState_subid00;
    if (target == 0x4857) goto normalState_subid01;
    if (target == 0x4877) goto normalState_subid02;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(0x47d4, enemyCode0b_setRandomCounter1_hook, 0x4942, 0x47d7);
  CYC(0x47d7, 0x47da); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp

state_switchHook:
  CYC(0x47da, 0x47db); E = alu_inc8(gb, E);
  CYC(0x47db, 0x47dc); A = mem_rd(gb, DE);
  {
    CYC(0x47dc, 0x47dd); push_effect(gb, 0x47dd);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x47e5) goto switchHook_substate1;
    if (target == 0x47e6) goto switchHook_substate3;
    HANDOFF(target);
  }

switchHook_substate1:
  RET(0x47e5); return; // ret

switchHook_substate3:
  CYC(0x47e6, 0x47e8); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x47e8, 0x47e9); A = mem_rd(gb, DE);
  CYC(0x47e9, 0x47ec); SET_HL(0x47f1); // @@destStates
  CYC(0x47ec, 0x47ed); enemyCode0b_addAToHl_from_rst(gb, 0x47ed);
  CYC(0x47ed, 0x47ee); B = mem_rd(gb, HL);
  CYC(0x47ee, 0x47f1); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp

state_stub:
  RET(0x47f4); return; // ret

normalState_subid00:
  CYC(0x47f5, 0x47f6); A = mem_rd(gb, DE);
  CYC(0x47f6, 0x47f8); alu_sub(gb, 0x08);
  {
    CYC(0x47f8, 0x47f9); push_effect(gb, 0x47f9);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x4801) goto state8;
    if (target == 0x4817) goto state9;
    if (target == 0x4831) goto subid00_stateA;
    if (target == 0x4847) goto stateB;
    HANDOFF(target);
  }

state8:
  CALL_C(0x4801, ecom_decCounter1_b0d_hook, 0x439a, 0x4804);
  if (!(F & FZ)) { RET_TAKEN(0x4804); return; } // ret nz
  CYC(0x4804, 0x4805);
  CYC(0x4805, 0x4806); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x4806, enemyCode0b_chooseSpawnPosition_hook, 0x48f3, 0x4809);
  if (!(F & FZ)) { RET_TAKEN(0x4809); return; } // ret nz
  CYC(0x4809, 0x480a);
  CALL_C(0x480a, objectSetShortPosition_hook, 0x20c3, 0x480d);
  CYC(0x480d, 0x480f); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x480f, 0x4810); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4810, 0x4811); alu_xor(gb, A);
  CALL_C(0x4811, enemySetAnimation_hook, 0x282b, 0x4814);
  CYC(0x4814, 0x4817); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CYC(0x4817, 0x4818); H = D;
  CYC(0x4818, 0x481a); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x481a, 0x481b); A = mem_rd(gb, HL);
  CYC(0x481b, 0x481c); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x481c, 0x481e); goto animate; } // jr nz
  CYC(0x481c, 0x481e);
  CYC(0x481e, 0x481f); L = E;
  CYC(0x481f, 0x4820); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4820, 0x4822); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4822, 0x4824); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4824, 0x4826); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4826, 0x4828); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x4828, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x482b);
  CALL_C(0x482b, enemyCode0b_setRandomHighCounter1_hook, 0x4954, 0x482e);

animate:
  CYC(0x482e, 0x4831); enemyAnimate_hook(gb); return; // jp

subid00_stateA:
  CALL_C(0x4831, ecom_decCounter1_b0d_hook, 0x439a, 0x4834);
  if (!(F & FZ)) { CYCT(0x4834, 0x4837); goto updatePosition; } // jp nz
  CYC(0x4834, 0x4837);

backIntoGround:
  CALL_C(0x4837, ecom_incState_b0d_hook, 0x4000, 0x483a);
  CYC(0x483a, 0x483c); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x483c, 0x483e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x483e, 0x4840); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4840, 0x4842); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x4842, 0x4844); A = 0x02;
  CYC(0x4844, 0x4847); enemySetAnimation_hook(gb); return; // jp

stateB:
  CYC(0x4847, 0x4848); H = D;
  CYC(0x4848, 0x484a); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x484a, 0x484b); A = mem_rd(gb, HL);
  CYC(0x484b, 0x484c); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x484c, 0x484e); goto animate; } // jr nz
  CYC(0x484c, 0x484e);
  CYC(0x484e, 0x484f); L = E;
  CYC(0x484f, 0x4851); mem_wr(gb, HL, 0x08);
  CALL_C(0x4851, enemyCode0b_setRandomCounter1_hook, 0x4942, 0x4854);
  CYC(0x4854, 0x4857); objectSetInvisible_hook(gb); return; // jp

normalState_subid01:
  CYC(0x4857, 0x4858); A = mem_rd(gb, DE);
  CYC(0x4858, 0x485a); alu_sub(gb, 0x08);
  {
    CYC(0x485a, 0x485b); push_effect(gb, 0x485b);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x4801) goto state8;
    if (target == 0x4817) goto state9;
    if (target == 0x4863) goto subid01_stateA;
    if (target == 0x4847) goto stateB;
    HANDOFF(target);
  }

subid01_stateA:
  CALL_C(0x4863, ecom_decCounter1_b0d_hook, 0x439a, 0x4866);
  if (F & FZ) { CYCT(0x4866, 0x4869); goto backIntoGround; } // jp z
  CYC(0x4866, 0x4869);
  CALL_C(0x4869, getRandomNumber_noPreserveVars_hook, 0x0453, 0x486c);
  CYC(0x486c, 0x486e); alu_cp(gb, 0x14);
  if (!(F & FC)) { CYCT(0x486e, 0x4871); goto updatePosition; } // jp nc
  CYC(0x486e, 0x4871);
  CALL_C(0x4871, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x4874);
  CYC(0x4874, 0x4877); goto updatePosition; // jp

normalState_subid02:
  CYC(0x4877, 0x4878); A = mem_rd(gb, DE);
  CYC(0x4878, 0x487a); alu_sub(gb, 0x08);
  {
    CYC(0x487a, 0x487b); push_effect(gb, 0x487b);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == 0x4885) goto subid02_state8;
    if (target == 0x489b) goto subid02_state9;
    if (target == 0x48ab) goto subid02_stateA;
    if (target == 0x48c5) goto subid02_stateB;
    if (target == 0x48d4) goto subid02_stateC;
    HANDOFF(target);
  }

subid02_state8:
  CYC(0x4885, 0x4886); H = D;
  CYC(0x4886, 0x4887); L = E;
  CYC(0x4887, 0x4888); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4888, 0x488a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x488a, 0x488b); A = mem_rd(gb, HL);
  CYC(0x488b, 0x488d); alu_and(gb, 0x30);
  CYC(0x488d, 0x488f); alu_add(gb, 0x60);
  CYC(0x488f, 0x4890); mem_wr(gb, HL, A);
  CYC(0x4890, 0x4892); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4892, 0x4894); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4894, 0x4895); A = mem_rd(gb, DE);
  CYC(0x4895, 0x4896); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4896, 0x4898); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4898, 0x4899); A = mem_rd(gb, DE);
  CYC(0x4899, 0x489a); mem_wr(gb, HL, A);
  RET(0x489a); return; // ret

subid02_state9:
  CALL_C(0x489b, ecom_decCounter1_b0d_hook, 0x439a, 0x489e);
  if (!(F & FZ)) { RET_TAKEN(0x489e); return; } // ret nz
  CYC(0x489e, 0x489f);
  CYC(0x489f, 0x48a0); L = alu_inc8(gb, L);
  CYC(0x48a0, 0x48a2); mem_wr(gb, HL, 0x06); // [counter2] = 6
  CYC(0x48a2, 0x48a3); L = E;
  CYC(0x48a3, 0x48a4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x48a4, 0x48a5); alu_xor(gb, A);
  CALL_C(0x48a5, enemySetAnimation_hook, 0x282b, 0x48a8);
  CYC(0x48a8, 0x48ab); objectSetVisiblec2_hook(gb); return; // jp

subid02_stateA:
  CYC(0x48ab, 0x48ad); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x48ad, 0x48ae); A = mem_rd(gb, DE);
  CYC(0x48ae, 0x48af); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x48af, 0x48b1); goto animate2; } // jr nz
  CYC(0x48af, 0x48b1);
  CYC(0x48b1, 0x48b2); H = D;
  CYC(0x48b2, 0x48b4); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x48b4, 0x48b5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x48b5, 0x48b7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x48b7, 0x48b9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x48b9, 0x48bb); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x48bb, 0x48bd); mem_wr(gb, HL, 0x19); // SPEED_a0
  CALL_C(0x48bd, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x48c0);
  CALL_C(0x48c0, enemyCode0b_setRandomHighCounter1_hook, 0x4954, 0x48c3);
  CYC(0x48c3, 0x48c5); goto animate2; // jr

subid02_stateB:
  CALL_C(0x48c5, ecom_decCounter1_b0d_hook, 0x439a, 0x48c8);
  if (F & FZ) { CYCT(0x48c8, 0x48cb); goto backIntoGround; } // jp z
  CYC(0x48c8, 0x48cb);
  CALL_C(0x48cb, enemyCode0b_nudgeTowardsLink_hook, 0x495f, 0x48ce);
  CALL_C(0x48ce, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x48d1);

animate2:
  CYC(0x48d1, 0x48d4); enemyAnimate_hook(gb); return; // jp

subid02_stateC:
  CYC(0x48d4, 0x48d6); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x48d6, 0x48d7); A = mem_rd(gb, DE);
  CYC(0x48d7, 0x48d8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x48d8, 0x48da); goto animate2; } // jr nz
  CYC(0x48d8, 0x48da);
  CYC(0x48da, 0x48dc); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x48dc, 0x48de); A = 0x09;
  CYC(0x48de, 0x48df); mem_wr(gb, DE, A);
  CALL_C(0x48df, enemyCode0b_setRandomCounter1_hook, 0x4942, 0x48e2);
  CYC(0x48e2, 0x48e5); objectSetInvisible_hook(gb); return; // jp

updatePosition:
  CYC(0x48e5, 0x48e7); A = 0x01;
  CALL_C(0x48e7, ecom_getTopDownAdjacentWallsBitset_b0d_hook, 0x4204, 0x48ea);
  if (!(F & FZ)) { CYCT(0x48ea, 0x48ed); goto backIntoGround; } // jp nz
  CYC(0x48ea, 0x48ed);
  CALL_C(0x48ed, objectApplySpeed_hook, 0x201d, 0x48f0);
  CYC(0x48f0, 0x48f3); enemyAnimate_hook(gb); return; // jp
}
