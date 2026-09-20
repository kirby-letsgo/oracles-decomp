#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode13), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode13), (from), (to), true)

void spark_updateAngle_hook(GB *gb);
void spark_getWallAngle_hook(GB *gb);
void spark_state_uninitialized_hook(GB *gb);
void spark_state_stub_hook(GB *gb);
void spark_state8_hook(GB *gb);
void spark_state9_hook(GB *gb);
void spark_stateA_hook(GB *gb);

static uint16_t enemyCode13_jump_table(GB *gb) {
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
// ENEMY_SPARK
// ==================================================================================================
void enemyCode13_hook(GB *gb) {
  BASE(enemyCode13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+13, b_+15); alu_sub(gb, 0x17); // ITEMCOLLISION_L1_BOOMERANG
  CYC(b_+15, b_+17); alu_cp(gb, 0x01); // MAX_BOOMERANG_LEVEL
  if (!(F & FC)) { CYCT(b_+17, b_+19); goto normalStatus; } // jr nc
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto normalStatus; } // jr nc
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x09;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  {
    CYC(b_+32, b_+33); push_effect(gb, b_+33);
    uint16_t target = enemyCode13_jump_table(gb);
    if (target == SYM(spark_state_uninitialized)) { spark_state_uninitialized_hook(gb); return; }
    if (target == SYM(spark_state_stub)) { spark_state_stub_hook(gb); return; }
    if (target == SYM(spark_state8)) { spark_state8_hook(gb); return; }
    if (target == SYM(spark_state9)) { spark_state9_hook(gb); return; }
    if (target == SYM(spark_stateA)) { spark_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:50ba, bare global.
void spark_state_uninitialized_hook(GB *gb) {
  BASE(spark_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, spark_getWallAngle_hook, SYM(spark_getWallAngle), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); A = 0x28; // SPEED_100
  CALL_C(b_+8, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+11);
  CYC(b_+11, SYM(spark_state_stub)); objectSetVisible82_hook(gb); return; // jp
}

// 0d:50c8, bare global.
void spark_state_stub_hook(GB *gb) {
  BASE(spark_state_stub);
  RET(b_+0); return; // ret
}

// 0d:50c9, bare global; standard movement state.
void spark_state8_hook(GB *gb) {
  BASE(spark_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, spark_updateAngle_hook, SYM(spark_updateAngle), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CYC(b_+6, SYM(spark_state9)); enemyAnimate_hook(gb); return; // jp
}

// 0d:50d2, bare global; just hit by a boomerang (also whisp's state 9).
void spark_state9_hook(GB *gb) {
  BASE(spark_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC((SYM(initializeVramMap1) + 21)); // INTERAC_PUFF, subid 2
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+9, b_+11); A = 0x40; // Interaction.start
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); E = alu_inc8(gb, E);
  CYC(b_+13, b_+14); A = H;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+18);
  CYC(b_+18, SYM(spark_stateA)); objectSetInvisible_hook(gb); return; // jp
}

// 0d:50e7, bare global; deletes self and creates fairy when the "puff" is gone
// (also whisp's state A).
void spark_stateA_hook(GB *gb) {
  BASE(spark_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x13); // ENEMY_SPARK
  CYC(b_+13, b_+15); B = 0x01; // PART_ITEM_DROP
  if (F & FZ) { CALL_C(b_+15, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+18); } else { CYC(b_+15, b_+18); } // call z
  CYC(b_+18, SYM(enemyCode19)); enemyDelete_hook(gb); return; // jp
}
