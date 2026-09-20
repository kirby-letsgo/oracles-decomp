#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void tingle_createGlowAroundLink_hook(GB *gb) {
  BASE(tingle_createGlowAroundLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x8404);
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x46;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x78);
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_yh);
  CYC(b_+14, b_+16); L = 0x4b;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Link_xh);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}
