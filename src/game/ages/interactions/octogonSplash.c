#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// INTERAC_OCTOGON_SPLASH
void interactionCode8e_hook(GB *gb) {
  BASE(interactionCode8e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);

// interactionCode8e@state1
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); interactionAnimate_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); interactionDelete_hook(gb); return; // jp

state0:
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  CALL_C(b_+19, interactionIncState_hook, SYM(interactionIncState), b_+22);
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_rrca(gb);
  CYC(b_+26, b_+27); alu_rrca(gb);
  CALL_C(b_+27, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+30);
  CYC(b_+30, b_+33); objectSetVisible81_hook(gb); return; // jp
}
