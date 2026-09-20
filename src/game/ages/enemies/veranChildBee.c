#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode1f);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockbackNoSolidity_b0d_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  RET(b_+12); return;

normalStatus:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  {
    uint16_t target = enemyCode1f_jump_table(gb);
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+66) goto state8;
    if (target == b_+83) goto state9;
    if (target == b_+93) goto stateA;
    if (target == b_+39) goto stateUninitialized;
    RET(b_+65); return; // states 1-4, 6, 7 all target 0x6c8d, a bare `ret`
  }

stateUninitialized:
  CYC(b_+39, b_+41); A = 0x50; // SPEED_200
  CALL_C(b_+41, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+44);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x10);
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+54); SET_HL(b_+62); // @angleVals
  CYC(b_+54, b_+55); enemyCode1f_addAToHl_from_rst(gb, b_+55);
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+62); objectSetVisible83_hook(gb); return; // jp

  // enemyCode1f@angleVals (0d:6c8a-6c8c): pure data ($10 $16 $0a), read only through the
  // addAToHl indexing above.

state8:
  CALL_C(b_+66, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+69);
  if (F & FZ) { CYCT(b_+69, b_+71); goto counter1AndState; } // jr z
  CYC(b_+69, b_+71);
  CALL_C(b_+71, objectApplySpeed_hook, SYM(objectApplySpeed), b_+74);
  CYC(b_+74, b_+76); goto animate; // jr

counter1AndState:
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+78, b_+79); L = E;
  CYC(b_+79, b_+80); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  // falls into @animate

animate:
  CYC(b_+80, b_+83); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(b_+83, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+86);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto animate; } // jr nz
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); L = E;
  CYC(b_+89, b_+90); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+90, ecom_updateAngleTowardTarget_b0d_hook, SYM(ecom_updateAngleTowardTarget_b0d), b_+93);
  // falls through to stateA

stateA:
  CALL_C(b_+93, objectApplySpeed_hook, SYM(objectApplySpeed), b_+96);
  CALL_C(b_+96, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+99);
  if (F & FC) { CYCT(b_+99, b_+101); goto animate; } // jr c
  CYC(b_+99, b_+101);
  CALL_C(b_+101, decNumEnemies_hook, SYM(decNumEnemies), b_+104);
  CYC(b_+104, b_+107); enemyDelete_hook(gb); return; // jp
}
