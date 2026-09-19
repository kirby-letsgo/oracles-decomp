#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void whisp_state_uninitialized_hook(GB *gb);
void whisp_state8_hook(GB *gb);
void spark_checkWallInDirection_hook(GB *gb);
void spark_getTileOffset_hook(GB *gb);
void spark_state_stub_hook(GB *gb);
void spark_state9_hook(GB *gb);
void spark_stateA_hook(GB *gb);

static uint16_t enemyCode19_jump_table(GB *gb) {
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

static void whisp_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_WHISP
// ==================================================================================================
void enemyCode19_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x50fc, 0x50fe); goto normalStatus; } // jr z
  CYC(0x50fc, 0x50fe);
  CYC(0x50fe, 0x5100); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5100); return; } // ret c
  CYC(0x5100, 0x5101);
  CYC(0x5101, 0x5103); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x5103, 0x5104); A = mem_rd(gb, DE);
  CYC(0x5104, 0x5106); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x5106, 0x5108); alu_sub(gb, 0x17); // ITEMCOLLISION_L1_BOOMERANG
  CYC(0x5108, 0x510a); alu_cp(gb, 0x01); // MAX_BOOMERANG_LEVEL
  if (!(F & FC)) { CYCT(0x510a, 0x510c); goto normalStatus; } // jr nc
  CYC(0x510a, 0x510c);
  CYC(0x510c, 0x510e); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x510e, 0x510f); A = mem_rd(gb, DE);
  CYC(0x510f, 0x5111); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x5111, 0x5113); goto normalStatus; } // jr nc
  CYC(0x5111, 0x5113);
  CYC(0x5113, 0x5115); A = 0x09;
  CYC(0x5115, 0x5116); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x5116, 0x5118); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5118, 0x5119); A = mem_rd(gb, DE);
  {
    CYC(0x5119, 0x511a); push_effect(gb, 0x511a);
    uint16_t target = enemyCode19_jump_table(gb);
    if (target == 0x5130) { whisp_state_uninitialized_hook(gb); return; }
    if (target == 0x50c8) { spark_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5142) { whisp_state8_hook(gb); return; }
    if (target == 0x50d2) { spark_state9_hook(gb); return; }
    if (target == 0x50e7) { spark_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5130, bare global.
void whisp_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5130, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5133);
  CYC(0x5133, 0x5135); alu_and(gb, 0x18);
  CYC(0x5135, 0x5137); alu_add(gb, 0x04);
  CYC(0x5137, 0x5139); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5139, 0x513a); mem_wr(gb, DE, A);
  CYC(0x513a, 0x513c); A = 0x1e; // SPEED_c0
  CALL_C(0x513c, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x513f);
  CYC(0x513f, 0x5142); objectSetVisible82_hook(gb); return; // jp
}

// 0d:5142, bare global.
void whisp_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5142, ecom_bounceOffWalls_b0d_hook, 0x42e2, 0x5145);
  CALL_C(0x5145, objectApplySpeed_hook, 0x201d, 0x5148);
  CYC(0x5148, 0x514b); enemyAnimate_hook(gb); return; // jp
}

// 0d:514b, bare global; updates the spark's moving angle by checking for walls.
// Sparks (and whisps) move by hugging walls.
void spark_updateAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x514b, 0x514d); A = 0x01;
  CYC(0x514d, 0x514f); hram_wr(gb, 0x8a, A);
  CYC(0x514f, 0x5151); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5151, 0x5152); A = mem_rd(gb, DE);
  CYC(0x5152, 0x5154); alu_sub(gb, 0x08);
  CYC(0x5154, 0x5156); alu_and(gb, 0x18);
  CALL_C(0x5156, spark_checkWallInDirection_hook, 0x51a0, 0x5159);
  if (F & FC) { CYCT(0x5159, 0x515b); goto stillOnWall; } // jr c
  CYC(0x5159, 0x515b);
  CALL_C(0x515b, spark_getTileOffset_hook, 0x518d, 0x515e);
  if (!(F & FZ)) { RET_TAKEN(0x515e); return; } // ret nz
  CYC(0x515e, 0x515f);
  CYC(0x515f, 0x5161); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5161, 0x5162); A = mem_rd(gb, DE);
  CYC(0x5162, 0x5164); alu_sub(gb, 0x08);
  CYC(0x5164, 0x5166); alu_and(gb, 0x18);
  CYC(0x5166, 0x5167); mem_wr(gb, DE, A);
  RET(0x5167); return; // ret

stillOnWall:
  CYC(0x5168, 0x516a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x516a, 0x516b); A = mem_rd(gb, DE);
  CALL_C(0x516b, spark_checkWallInDirection_hook, 0x51a0, 0x516e);
  if (!(F & FC)) { RET_TAKEN(0x516e); return; } // ret nc
  CYC(0x516e, 0x516f);
  CYC(0x516f, 0x5171); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5171, 0x5172); A = mem_rd(gb, DE);
  CYC(0x5172, 0x5174); alu_add(gb, 0x08);
  CYC(0x5174, 0x5176); alu_and(gb, 0x18);
  CYC(0x5176, 0x5177); mem_wr(gb, DE, A);
  RET(0x5177); return; // ret
}

// 0d:5178, bare global.
void spark_getWallAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5178, 0x5179); alu_xor(gb, A);
  CALL_C(0x5179, spark_checkWallInDirection_hook, 0x51a0, 0x517c);
  CYC(0x517c, 0x517e); A = 0x08;
  if (F & FC) { RET_TAKEN(0x517e); return; } // ret c
  CYC(0x517e, 0x517f);
  CALL_C(0x517f, spark_checkWallInDirection_hook, 0x51a0, 0x5182);
  CYC(0x5182, 0x5184); A = 0x10;
  if (F & FC) { RET_TAKEN(0x5184); return; } // ret c
  CYC(0x5184, 0x5185);
  CALL_C(0x5185, spark_checkWallInDirection_hook, 0x51a0, 0x5188);
  CYC(0x5188, 0x518a); A = 0x18;
  if (F & FC) { RET_TAKEN(0x518a); return; } // ret c
  CYC(0x518a, 0x518b);
  CYC(0x518b, 0x518c); alu_xor(gb, A);
  RET(0x518c); return; // ret
}

// 0d:518d, bare global.
void spark_getTileOffset_hook(GB *gb) {
  CYC(0x518d, 0x518f); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x518f, 0x5190); A = mem_rd(gb, DE);
  CYC(0x5190, 0x5192); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(0x5192, 0x5194); goto xOffset; } // jr nz
  CYC(0x5192, 0x5194);
  CYC(0x5194, 0x5196); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5196, 0x5197); A = mem_rd(gb, DE);
  CYC(0x5197, 0x5199); alu_and(gb, 0x07);
  RET(0x5199); return; // ret

xOffset:
  CYC(0x519a, 0x519c); E = ENEMY_BASE + OBJ_XH;
  CYC(0x519c, 0x519d); A = mem_rd(gb, DE);
  CYC(0x519d, 0x519f); alu_and(gb, 0x07);
  RET(0x519f); return; // ret
}

// 0d:51a0, bare global.
void spark_checkWallInDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51a0, 0x51a2); alu_and(gb, 0x18);
  CYC(0x51a2, 0x51a3); alu_rrca(gb);
  CYC(0x51a3, 0x51a6); SET_HL(0x51c4); // @offsetTable (data)
  CYC(0x51a6, 0x51a7); whisp_addAToHl_from_rst(gb, 0x51a7);
  CYC(0x51a7, 0x51a9); E = ENEMY_BASE + OBJ_YH;
  CYC(0x51a9, 0x51aa); A = mem_rd(gb, DE);
  CYC(0x51aa, 0x51ab); alu_add(gb, mem_rd(gb, HL));
  CYC(0x51ab, 0x51ac); B = A;
  CYC(0x51ac, 0x51ad); SET_HL(HL + 1); // inc hl
  CYC(0x51ad, 0x51af); E = ENEMY_BASE + OBJ_XH;
  CYC(0x51af, 0x51b0); A = mem_rd(gb, DE);
  CYC(0x51b0, 0x51b1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x51b1, 0x51b2); C = A;
  PUSH(0x51b2, HL);
  PUSH(0x51b3, BC);
  CALL_C(0x51b4, checkTileCollisionAt_disallowHoles_hook, 0x1502, 0x51b7);
  SET_BC(POP(0x51b7));
  SET_HL(POP(0x51b8));
  if (F & FC) { RET_TAKEN(0x51b9); return; } // ret c
  CYC(0x51b9, 0x51ba);
  CYC(0x51ba, 0x51bb); SET_HL(HL + 1); // inc hl
  CYC(0x51bb, 0x51bc); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x51bc, 0x51bd); alu_add(gb, B);
  CYC(0x51bd, 0x51be); B = A;
  CYC(0x51be, 0x51bf); A = mem_rd(gb, HL);
  CYC(0x51bf, 0x51c0); alu_add(gb, C);
  CYC(0x51c0, 0x51c1); C = A;
  CYC(0x51c1, 0x51c4); checkTileCollisionAt_disallowHoles_hook(gb); return; // jp
}
