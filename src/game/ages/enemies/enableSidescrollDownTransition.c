#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode2b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode2b), (from), (to), true)

// ==================================================================================================
// ENEMY_ENABLE_SIDESCROLL_DOWN_TRANSITION
// ==================================================================================================
void enemyCode2b_hook(GB *gb) {
  BASE(enemyCode2b);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); ecom_incState_b0d_hook(gb); return; } // jp z
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+10); SET_HL(w1Link_xh);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0xd0);
  if (F & FC) { CYCT(b_+13, b_+14); ret_effect(gb); return; } // ret c
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = (uint8_t)w1Link_yh;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); L = (uint8_t)(w1Link_speedZ + 1);
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+22); alu_cp(gb, 0xa8); // (LARGE_ROOM_HEIGHT << 4) - 8
  if (F & FC) { CYCT(b_+22, b_+23); ret_effect(gb); return; } // ret c
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); A = 0x82; // $80 | DIR_DOWN
  CYC(b_+25, b_+28); W8(wScreenTransitionDirection) = A;
  RET(b_+28); return;
}
