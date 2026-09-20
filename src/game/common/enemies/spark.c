#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5083, ecom_checkHazards_b0d_hook, 0x4051, 0x5086);
  if (F & FZ) { CYCT(0x5086, 0x5088); goto normalStatus; } // jr z
  CYC(0x5086, 0x5088);
  CYC(0x5088, 0x508a); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x508a); return; } // ret c
  CYC(0x508a, 0x508b);
  CYC(0x508b, 0x508d); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x508d, 0x508e); A = mem_rd(gb, DE);
  CYC(0x508e, 0x5090); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x5090, 0x5092); alu_sub(gb, 0x17); // ITEMCOLLISION_L1_BOOMERANG
  CYC(0x5092, 0x5094); alu_cp(gb, 0x01); // MAX_BOOMERANG_LEVEL
  if (!(F & FC)) { CYCT(0x5094, 0x5096); goto normalStatus; } // jr nc
  CYC(0x5094, 0x5096);
  CYC(0x5096, 0x5098); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5098, 0x5099); A = mem_rd(gb, DE);
  CYC(0x5099, 0x509b); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x509b, 0x509d); goto normalStatus; } // jr nc
  CYC(0x509b, 0x509d);
  CYC(0x509d, 0x509f); A = 0x09;
  CYC(0x509f, 0x50a0); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x50a0, 0x50a2); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x50a2, 0x50a3); A = mem_rd(gb, DE);
  {
    CYC(0x50a3, 0x50a4); push_effect(gb, 0x50a4);
    uint16_t target = enemyCode13_jump_table(gb);
    if (target == 0x50ba) { spark_state_uninitialized_hook(gb); return; }
    if (target == 0x50c8) { spark_state_stub_hook(gb); return; }
    if (target == 0x50c9) { spark_state8_hook(gb); return; }
    if (target == 0x50d2) { spark_state9_hook(gb); return; }
    if (target == 0x50e7) { spark_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:50ba, bare global.
void spark_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50ba, spark_getWallAngle_hook, 0x5178, 0x50bd);
  CYC(0x50bd, 0x50bf); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x50bf, 0x50c0); mem_wr(gb, DE, A);
  CYC(0x50c0, 0x50c2); A = 0x28; // SPEED_100
  CALL_C(0x50c2, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x50c5);
  CYC(0x50c5, 0x50c8); objectSetVisible82_hook(gb); return; // jp
}

// 0d:50c8, bare global.
void spark_state_stub_hook(GB *gb) {
  RET(0x50c8); return; // ret
}

// 0d:50c9, bare global; standard movement state.
void spark_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50c9, spark_updateAngle_hook, 0x514b, 0x50cc);
  CALL_C(0x50cc, objectApplySpeed_hook, 0x201d, 0x50cf);
  CYC(0x50cf, 0x50d2); enemyAnimate_hook(gb); return; // jp
}

// 0d:50d2, bare global; just hit by a boomerang (also whisp's state 9).
void spark_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50d2, 0x50d5); SET_BC(0x0502); // INTERAC_PUFF, subid 2
  CALL_C(0x50d5, objectCreateInteraction_hook, 0x24c5, 0x50d8);
  if (!(F & FZ)) { RET_TAKEN(0x50d8); return; } // ret nz
  CYC(0x50d8, 0x50d9);
  CYC(0x50d9, 0x50db); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x50db, 0x50dd); A = 0x40; // Interaction.start
  CYC(0x50dd, 0x50de); mem_wr(gb, DE, A);
  CYC(0x50de, 0x50df); E = alu_inc8(gb, E);
  CYC(0x50df, 0x50e0); A = H;
  CYC(0x50e0, 0x50e1); mem_wr(gb, DE, A);
  CALL_C(0x50e1, ecom_incState_b0d_hook, 0x4000, 0x50e4);
  CYC(0x50e4, 0x50e7); objectSetInvisible_hook(gb); return; // jp
}

// 0d:50e7, bare global; deletes self and creates fairy when the "puff" is gone
// (also whisp's state A).
void spark_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50e7, 0x50e9); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x50e9, objectGetRelatedObject2Var_hook, 0x2164, 0x50ec);
  CYC(0x50ec, 0x50ed); A = mem_rd(gb, HL);
  CYC(0x50ed, 0x50ee); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x50ee); return; } // ret nz
  CYC(0x50ee, 0x50ef);
  CYC(0x50ef, 0x50f1); E = ENEMY_BASE + OBJ_ID;
  CYC(0x50f1, 0x50f2); A = mem_rd(gb, DE);
  CYC(0x50f2, 0x50f4); alu_cp(gb, 0x13); // ENEMY_SPARK
  CYC(0x50f4, 0x50f6); B = 0x01; // PART_ITEM_DROP
  if (F & FZ) { CALL_C(0x50f6, ecom_spawnProjectile_b0d_hook, 0x437c, 0x50f9); } else { CYC(0x50f6, 0x50f9); } // call z
  CYC(0x50f9, 0x50fc); enemyDelete_hook(gb); return; // jp
}
