#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t hardhatBeetle_jump_table(GB *gb) {
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
// ENEMY_HARDHAT_BEETLE
// ENEMY_HARMLESS_HARDHAT_BEETLE (ages only)
//
// enemyCode4d and enemyCode5f are the same bytes at the same address: ENEMY_HARMLESS_HARDHAT_BEETLE
// (ages only) shares its entire routine with ENEMY_HARDHAT_BEETLE.
// ==================================================================================================
void enemyCode4d_hook(GB *gb) {
  BASE(enemyCode4d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  RET(b_+15); return; // ret

normalStatus:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = hardhatBeetle_jump_table(gb);
    if (target == b_+38) goto state_uninitialized;
    if (target == b_+53) { RET(b_+53); return; } // ret (state_stub)
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == b_+54) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+43); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  CYC(b_+43, b_+45); A = 0x8d; // PALH_8d
  if (F & FZ) CALL_C_CC(b_+45, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+48); else CYC(b_+45, b_+48); // call z
  CYC(b_+48, b_+50); A = 0x0f; // SPEED_60
  CYC(b_+50, b_+53); TAIL(ecom_setSpeedAndState8AndVisible_b0e); // jp

state8:
  CALL_C(b_+54, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+57);
  CALL_C(b_+57, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+60);
  CYC(b_+60, b_+63); TAIL(enemyAnimate); // jp
}

// 0e:60bb, alias of enemyCode4d (ENEMY_HARMLESS_HARDHAT_BEETLE, ages only): identical bytes at
// the identical address.
void enemyCode5f_hook(GB *gb) {
  TAIL(enemyCode4d);
}
