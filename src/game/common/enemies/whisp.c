#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode19);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+10, b_+12); alu_sub(gb, GV(0x17, 0x15)); // ITEMCOLLISION_L1_BOOMERANG
  CYC(b_+12, b_+14); alu_cp(gb, GV(0x01, 0x02)); // MAX_BOOMERANG_LEVEL
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto normalStatus; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto normalStatus; } // jr nc
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x09;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  {
    CYC(b_+29, b_+30); push_effect(gb, b_+30);
    uint16_t target = enemyCode19_jump_table(gb);
    if (target == SYM(whisp_state_uninitialized)) { whisp_state_uninitialized_hook(gb); return; }
    if (target == SYM(spark_state_stub)) { spark_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(whisp_state8)) { whisp_state8_hook(gb); return; }
    if (target == SYM(spark_state9)) { spark_state9_hook(gb); return; }
    if (target == SYM(spark_stateA)) { spark_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5130, bare global.
void whisp_state_uninitialized_hook(GB *gb) {
  BASE(whisp_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+7); alu_add(gb, 0x04);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); A = 0x1e; // SPEED_c0
  CALL_C(b_+12, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+15);
  CYC(b_+15, b_+18); objectSetVisible82_hook(gb); return; // jp
}

// 0d:5142, bare global.
void whisp_state8_hook(GB *gb) {
  BASE(whisp_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_bounceOffWalls_b0d_hook, SYM(ecom_bounceOffWalls_b0d), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CYC(b_+6, b_+9); enemyAnimate_hook(gb); return; // jp
}

// 0d:514b, bare global; updates the spark's moving angle by checking for walls.
// Sparks (and whisps) move by hugging walls.
void spark_updateAngle_hook(GB *gb) {
  BASE(spark_updateAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); mem_wr(gb, hFF8A, A);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_sub(gb, 0x08);
  CYC(b_+9, b_+11); alu_and(gb, 0x18);
  CALL_C(b_+11, spark_checkWallInDirection_hook, SYM(spark_checkWallInDirection), b_+14);
  if (F & FC) { CYCT(b_+14, b_+16); goto stillOnWall; } // jr c
  CYC(b_+14, b_+16);
  CALL_C(b_+16, spark_getTileOffset_hook, SYM(spark_getTileOffset), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_sub(gb, 0x08);
  CYC(b_+25, b_+27); alu_and(gb, 0x18);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret

stillOnWall:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CALL_C(b_+32, spark_checkWallInDirection_hook, SYM(spark_checkWallInDirection), b_+35);
  if (!(F & FC)) { RET_TAKEN(b_+35); return; } // ret nc
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); alu_add(gb, 0x08);
  CYC(b_+41, b_+43); alu_and(gb, 0x18);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  RET(b_+44); return; // ret
}

// 0d:5178, bare global.
void spark_getWallAngle_hook(GB *gb) {
  BASE(spark_getWallAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, spark_checkWallInDirection_hook, SYM(spark_checkWallInDirection), b_+4);
  CYC(b_+4, b_+6); A = 0x08;
  if (F & FC) { RET_TAKEN(b_+6); return; } // ret c
  CYC(b_+6, b_+7);
  CALL_C(b_+7, spark_checkWallInDirection_hook, SYM(spark_checkWallInDirection), b_+10);
  CYC(b_+10, b_+12); A = 0x10;
  if (F & FC) { RET_TAKEN(b_+12); return; } // ret c
  CYC(b_+12, b_+13);
  CALL_C(b_+13, spark_checkWallInDirection_hook, SYM(spark_checkWallInDirection), b_+16);
  CYC(b_+16, b_+18); A = 0x18;
  if (F & FC) { RET_TAKEN(b_+18); return; } // ret c
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  RET(b_+20); return; // ret
}

// 0d:518d, bare global.
void spark_getTileOffset_hook(GB *gb) {
  BASE(spark_getTileOffset);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto xOffset; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_and(gb, 0x07);
  RET(b_+12); return; // ret

xOffset:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_and(gb, 0x07);
  RET(b_+18); return; // ret
}

// 0d:51a0, bare global.
void spark_checkWallInDirection_hook(GB *gb) {
  BASE(spark_checkWallInDirection);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_and(gb, 0x18);
  CYC(b_+2, b_+3); alu_rrca(gb);
  CYC(b_+3, b_+6); SET_HL(b_+36); // @offsetTable (data)
  CYC(b_+6, b_+7); whisp_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); SET_HL(HL + 1); // inc hl
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); C = A;
  PUSH(b_+18, HL);
  PUSH(b_+19, BC);
  CALL_C(b_+20, checkTileCollisionAt_disallowHoles_hook, SYM(checkTileCollisionAt_disallowHoles), b_+23);
  SET_BC(POP(b_+23));
  SET_HL(POP(b_+24));
  if (F & FC) { RET_TAKEN(b_+25); return; } // ret c
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); SET_HL(HL + 1); // inc hl
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+28, b_+29); alu_add(gb, B);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_add(gb, C);
  CYC(b_+32, b_+33); C = A;
  CYC(b_+33, b_+36); checkTileCollisionAt_disallowHoles_hook(gb); return; // jp
}
