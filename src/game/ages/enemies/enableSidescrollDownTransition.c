#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

// ==================================================================================================
// ENEMY_ENABLE_SIDESCROLL_DOWN_TRANSITION
// ==================================================================================================
void enemyCode2b_hook(GB *gb) {
  CYC(0x6d3f, 0x6d41); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d41, 0x6d42); A = mem_rd(gb, DE);
  CYC(0x6d42, 0x6d43); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6d43, 0x6d46); ecom_incState_b0d_hook(gb); return; } // jp z
  CYC(0x6d43, 0x6d46);
  CYC(0x6d46, 0x6d49); SET_HL(w1Link_xh);
  CYC(0x6d49, 0x6d4a); A = mem_rd(gb, HL);
  CYC(0x6d4a, 0x6d4c); alu_cp(gb, 0xd0);
  if (F & FC) { CYCT(0x6d4c, 0x6d4d); ret_effect(gb); return; } // ret c
  CYC(0x6d4c, 0x6d4d);
  CYC(0x6d4d, 0x6d4f); L = (uint8_t)w1Link_yh;
  CYC(0x6d4f, 0x6d50); A = mem_rd(gb, HL);
  CYC(0x6d50, 0x6d52); L = (uint8_t)(w1Link_speedZ + 1);
  CYC(0x6d52, 0x6d53); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6d53, 0x6d55); alu_cp(gb, 0xa8); // (LARGE_ROOM_HEIGHT << 4) - 8
  if (F & FC) { CYCT(0x6d55, 0x6d56); ret_effect(gb); return; } // ret c
  CYC(0x6d55, 0x6d56);
  CYC(0x6d56, 0x6d58); A = 0x82; // $80 | DIR_DOWN
  CYC(0x6d58, 0x6d5b); W8(wScreenTransitionDirection) = A;
  RET(0x6d5b); return;
}
