#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60bb, ecom_checkHazards_b0e_hook, 0x4051, 0x60be);
  if (F & FZ) { CYCT(0x60be, 0x60c0); goto normalStatus; } // jr z
  CYC(0x60be, 0x60c0);
  CYC(0x60c0, 0x60c2); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x60c2); return; } // ret c
  CYC(0x60c2, 0x60c3);
  if (F & FZ) { CYCT(0x60c3, 0x60c6); enemyDie_hook(gb); return; } // jp z
  CYC(0x60c3, 0x60c6);
  CYC(0x60c6, 0x60c7); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x60c7, 0x60ca); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x60c7, 0x60ca);
  RET(0x60ca); return; // ret

normalStatus:
  CYC(0x60cb, 0x60cd); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x60cd, 0x60ce); A = mem_rd(gb, DE);
  {
    CYC(0x60ce, 0x60cf); push_effect(gb, 0x60cf);
    uint16_t target = hardhatBeetle_jump_table(gb);
    if (target == 0x60e1) goto state_uninitialized;
    if (target == 0x60f0) { RET(0x60f0); return; } // ret (state_stub)
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x60f1) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x60e1, 0x60e3); E = ENEMY_BASE + OBJ_ID;
  CYC(0x60e3, 0x60e4); A = mem_rd(gb, DE);
  CYC(0x60e4, 0x60e6); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  CYC(0x60e6, 0x60e8); A = 0x8d; // PALH_8d
  if (F & FZ) CALL_C_CC(0x60e8, loadPaletteHeader_hook, 0x050b, 0x60eb); else CYC(0x60e8, 0x60eb); // call z
  CYC(0x60eb, 0x60ed); A = 0x0f; // SPEED_60
  CYC(0x60ed, 0x60f0); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp

state8:
  CALL_C(0x60f1, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x60f4);
  CALL_C(0x60f4, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x60f7);
  CYC(0x60f7, 0x60fa); enemyAnimate_hook(gb); return; // jp
}

// 0e:60bb, alias of enemyCode4d (ENEMY_HARMLESS_HARDHAT_BEETLE, ages only): identical bytes at
// the identical address.
void enemyCode5f_hook(GB *gb) {
  enemyCode4d_hook(gb);
}
