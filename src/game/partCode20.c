#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x537c, 0x537e); E = 0xc4; // Part.state
  CYC(0x537e, 0x537f); A = mem_rd(gb, DE);
  CYC(0x537f, 0x5380); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5380, 0x5382); goto state0; } // jr z
  CYC(0x5380, 0x5382);
  CALL_C(0x5382, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5385);
  if (F & FZ) { CYCT(0x5385, 0x5388); partDelete_hook(gb); return; } // jp z
  CYC(0x5385, 0x5388);
  CYC(0x5388, 0x538b); partAnimate_hook(gb); return; // jp

state0:
  CYC(0x538b, 0x538c); H = D;
  CYC(0x538c, 0x538d); L = E;
  CYC(0x538d, 0x538e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x538e, 0x5390); L = 0xc6; // Part.counter1
  CYC(0x5390, 0x5392); mem_wr(gb, HL, 0xb4);
  CYC(0x5392, 0x5395); objectSetVisible82_hook(gb); return; // jp
}
