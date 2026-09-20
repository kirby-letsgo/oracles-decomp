#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void tingle_createGlowAroundLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7990, 0x7993); SET_BC(0x8404);
  CALL_C(0x7993, objectCreateInteraction_hook, 0x24c5, 0x7996);
  if (!(F & FZ)) { RET_TAKEN(0x7996); return; }
  CYC(0x7996, 0x7997);
  CYC(0x7997, 0x7999); L = 0x46;
  CYC(0x7999, 0x799b); mem_wr(gb, HL, 0x78);
  CYC(0x799b, 0x799e); A = mem_rd(gb, w1Link_yh);
  CYC(0x799e, 0x79a0); L = 0x4b;
  CYC(0x79a0, 0x79a1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x79a1, 0x79a2); L = alu_inc8(gb, L);
  CYC(0x79a2, 0x79a5); A = mem_rd(gb, w1Link_xh);
  CYC(0x79a5, 0x79a6); mem_wr(gb, HL, A);
  CYC(0x79a6, 0x79a7); ret_effect(gb);
}
