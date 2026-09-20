#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode37), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode37), (from), (to), true)

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
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = butterfly_jump_table(gb);
    if (target == b_+8) goto state0;
    if (target == b_+21) goto state1;
    HANDOFF(target);
  }

state0:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+10); L = E;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x0a); // SPEED_40
  CALL_C(b_+15, ecom_setRandomAngle_b0e_hook, SYM(ecom_setRandomAngle_b0e), b_+18);
  CYC(b_+18, b_+21); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(b_+21, b_+24); SET_BC((SYM(pushDirectionData) + 26));
  CALL_C(b_+24, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+27);
  CYC(b_+27, b_+28); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto L_4ffb; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+33, b_+34); mem_wr(gb, HL, C);

L_4ffb:
  CALL_C(b_+34, objectApplySpeed_hook, SYM(objectApplySpeed), b_+37);
  CALL_C(b_+37, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+40);
  CYC(b_+40, SYM(enemyCode38)); enemyAnimate_hook(gb); return; // jp
}
