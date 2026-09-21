#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemInitializeFromLinkPosition_hook(GB *gb);

void itemCode0fPost_hook(GB *gb) {
  BASE(itemCode0fPost);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    TAIL(itemDelete);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+30);
  CALL_C(b_+9, itemInitializeFromLinkPosition_hook, SYM(itemInitializeFromLinkPosition), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+16); A = W8(w1Link_zh);
  CYC(b_+16, b_+18); L = 0x0f;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = 0x30;
  CYC(b_+21, b_+24); A = W8(w1ParentItem2_angle);
  CYC(b_+24, b_+25); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  if (F & FZ) {
    CYCT(b_+26, b_+27); ret_effect(gb); return;
  }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+30);
  itemSetAnimation_hook(gb);
}
