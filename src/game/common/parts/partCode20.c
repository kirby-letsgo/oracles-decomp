#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode20_hook(GB *gb) {
  BASE(partCode20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);
  CALL_C(b_+6, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); partDelete_hook(gb); return; } // jp z
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); TAIL(partAnimate); // jp

state0:
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+17); L = E;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+20); L = 0xc6; // Part.counter1
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xb4);
  CYC(b_+22, b_+25); TAIL(objectSetVisible82); // jp
}
