#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void twinrova_writePositionWithXOffsetTocfd5_hook(GB *gb);

void objectWritePositionTocfd5_hook(GB *gb) {
  BASE(objectWritePositionTocfd5);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  TAIL(twinrova_writePositionWithXOffsetTocfd5);
}

void twinrova_writePositionWithXOffsetTocfd5_hook(GB *gb) {
  BASE(twinrova_writePositionWithXOffsetTocfd5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CALL_C(b_+2, objectGetPosition_hook, SYM(objectGetPosition), b_+5);
  CYC(b_+5, b_+7); A = mem_rd(gb, hFF8B);
  CYC(b_+7, b_+10); SET_HL(wTmpcfc0_genericCutscene_cfd5);
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); alu_add(gb, C);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}
