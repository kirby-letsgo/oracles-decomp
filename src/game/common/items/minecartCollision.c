#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemCode1d_hook(GB *gb) {
  BASE(itemCode1d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+8);
  CALL_C(b_+8, itemIncState_hook, SYM(itemIncState), b_+11);
  CYC(b_+11, b_+13); L = 0x00;
  CYC(b_+13, b_+15); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void itemCode1dPost_hook(GB *gb) {
  BASE(itemCode1dPost);
  CYC(b_+0, b_+3); SET_HL(w1Companion_id);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+6, b_+9);
    objectTakePosition_hook(gb);
    return;
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+12);
  itemDelete_hook(gb);
}
