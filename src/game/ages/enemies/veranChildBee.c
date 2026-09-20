#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode1f_jump_table(GB *gb) {
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

static void enemyCode1f_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_VERAN_CHILD_BEE
//
// All of this enemy's states are @-local to enemyCode1f in the disassembly (no separate
// top-level symbols), so the entire state machine lives in this one hook.
// ==================================================================================================
void enemyCode1f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6c4c, 0x6c4e); goto normalStatus; } // jr z
  CYC(0x6c4c, 0x6c4e);
  CYC(0x6c4e, 0x6c50); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x6c50, 0x6c51); ret_effect(gb); return; } // ret c
  CYC(0x6c50, 0x6c51);
  if (F & FZ) { CYCT(0x6c51, 0x6c54); enemyDie_hook(gb); return; } // jp z
  CYC(0x6c51, 0x6c54);
  CYC(0x6c54, 0x6c55); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6c55, 0x6c58); ecom_updateKnockbackNoSolidity_b0d_hook(gb); return; } // jp nz
  CYC(0x6c55, 0x6c58);
  RET(0x6c58); return;

normalStatus:
  CYC(0x6c59, 0x6c5b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6c5b, 0x6c5c); A = mem_rd(gb, DE);
  CYC(0x6c5c, 0x6c5d); push_effect(gb, 0x6c5d);
  {
    uint16_t target = enemyCode1f_jump_table(gb);
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x6c8e) goto state8;
    if (target == 0x6c9f) goto state9;
    if (target == 0x6ca9) goto stateA;
    if (target == 0x6c73) goto stateUninitialized;
    RET(0x6c8d); return; // states 1-4, 6, 7 all target 0x6c8d, a bare `ret`
  }

stateUninitialized:
  CYC(0x6c73, 0x6c75); A = 0x50; // SPEED_200
  CALL_C(0x6c75, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x6c78);
  CYC(0x6c78, 0x6c7a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6c7a, 0x6c7c); mem_wr(gb, HL, 0x10);
  CYC(0x6c7c, 0x6c7e); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6c7e, 0x6c7f); A = mem_rd(gb, DE);
  CYC(0x6c7f, 0x6c82); SET_HL(0x6c8a); // @angleVals
  CYC(0x6c82, 0x6c83); enemyCode1f_addAToHl_from_rst(gb, 0x6c83);
  CYC(0x6c83, 0x6c85); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6c85, 0x6c86); A = mem_rd(gb, HL);
  CYC(0x6c86, 0x6c87); mem_wr(gb, DE, A);
  CYC(0x6c87, 0x6c8a); objectSetVisible83_hook(gb); return; // jp

  // enemyCode1f@angleVals (0d:6c8a-6c8c): pure data ($10 $16 $0a), read only through the
  // addAToHl indexing above.

state8:
  CALL_C(0x6c8e, ecom_decCounter1_b0d_hook, 0x439a, 0x6c91);
  if (F & FZ) { CYCT(0x6c91, 0x6c93); goto counter1AndState; } // jr z
  CYC(0x6c91, 0x6c93);
  CALL_C(0x6c93, objectApplySpeed_hook, 0x201d, 0x6c96);
  CYC(0x6c96, 0x6c98); goto animate; // jr

counter1AndState:
  CYC(0x6c98, 0x6c9a); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x6c9a, 0x6c9b); L = E;
  CYC(0x6c9b, 0x6c9c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  // falls into @animate

animate:
  CYC(0x6c9c, 0x6c9f); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(0x6c9f, ecom_decCounter1_b0d_hook, 0x439a, 0x6ca2);
  if (!(F & FZ)) { CYCT(0x6ca2, 0x6ca4); goto animate; } // jr nz
  CYC(0x6ca2, 0x6ca4);
  CYC(0x6ca4, 0x6ca5); L = E;
  CYC(0x6ca5, 0x6ca6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x6ca6, ecom_updateAngleTowardTarget_b0d_hook, 0x43bf, 0x6ca9);
  // falls through to stateA

stateA:
  CALL_C(0x6ca9, objectApplySpeed_hook, 0x201d, 0x6cac);
  CALL_C(0x6cac, objectCheckWithinRoomBoundary_hook, 0x219f, 0x6caf);
  if (F & FC) { CYCT(0x6caf, 0x6cb1); goto animate; } // jr c
  CYC(0x6caf, 0x6cb1);
  CALL_C(0x6cb1, decNumEnemies_hook, 0x24b3, 0x6cb4);
  CYC(0x6cb4, 0x6cb7); enemyDelete_hook(gb); return; // jp
}
