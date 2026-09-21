#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCode56_hook(GB *gb) {
  BASE(interactionCode56);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); goto L_495f;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = 0x61;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12); TAIL(interactionAnimate);
  }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); TAIL(interactionDelete);
L_495f:
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+20);
  CYC(b_+20, b_+22); A = 0x6f;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+27); E = 0x43;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+29, b_+32); TAIL(objectSetVisible81);
  }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+35); TAIL(objectSetVisible82);
}
