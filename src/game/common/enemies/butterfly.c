#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t butterfly_jump_table(GB *gb) {
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
// ENEMY_BUTTERFLY
// ==================================================================================================
void enemyCode37_hook(GB *gb) {
  BASE(enemyCode37);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = butterfly_jump_table(gb);
    if (target == b_+(game_seasons ? S(8) : 8)) goto state0;     // Seasons: state0 starts with the inserted wRoomStateModifier check
    if (target == b_+O(21)) goto state1;
    HANDOFF(target);
  }

state0:
  if (game_seasons) {
    CYC(b_+S(8), b_+S(11)); A = W8(wRoomStateModifier);
    CYC(b_+S(11), b_+S(12)); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+S(12), b_+S(15)); TAIL(enemyDelete); }
    CYC(b_+S(12), b_+S(15));
  }
  CYC(b_+O(8), b_+OE(9)); H = D;
  CYC(b_+O(9), b_+OE(10)); L = E;
  CYC(b_+O(10), b_+OE(11)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+O(11), b_+OE(13)); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+O(13), b_+OE(15)); mem_wr(gb, HL, 0x0a); // SPEED_40
  CALL_C(b_+O(15), ecom_setRandomAngle_b0e_hook, SYM(ecom_setRandomAngle_b0e), b_+OE(18));
  CYC(b_+O(18), b_+OE(21)); TAIL(objectSetVisible81); // jp

state1:
  CYC(b_+O(21), b_+OE(24)); SET_BC(0x1f1f);
  CALL_C(b_+O(24), ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+OE(27));
  CYC(b_+O(27), b_+OE(28)); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+O(28), b_+OE(30)); goto L_4ffb; } // jr nz
  CYC(b_+O(28), b_+OE(30));
  CYC(b_+O(30), b_+OE(31)); H = D;
  CYC(b_+O(31), b_+OE(33)); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+O(33), b_+OE(34)); mem_wr(gb, HL, C);

L_4ffb:
  CALL_C(b_+O(34), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(37));
  CALL_C(b_+O(37), ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+OE(40));
  CYC(b_+O(40), b_+OE(43)); TAIL(enemyAnimate); // jp
}
