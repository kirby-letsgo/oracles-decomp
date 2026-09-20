#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void twinrova_writePositionWithXOffsetTocfd5_hook(GB *gb);

void objectWritePositionTocfd5_hook(GB *gb) {
  CYC(0x741b, 0x741c); alu_xor(gb, A);
  twinrova_writePositionWithXOffsetTocfd5_hook(gb);
}

void twinrova_writePositionWithXOffsetTocfd5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x741c, 0x741e); hram_wr(gb, 0x8b, A);
  CALL_C(0x741e, objectGetPosition_hook, 0x208a, 0x7421);
  CYC(0x7421, 0x7423); A = hram_rd(gb, 0x8b);
  CYC(0x7423, 0x7426); SET_HL(0xcfd5);
  CYC(0x7426, 0x7427); mem_wr(gb, HL, B);
  CYC(0x7427, 0x7428); L = alu_inc8(gb, L);
  CYC(0x7428, 0x7429); alu_add(gb, C);
  CYC(0x7429, 0x742a); mem_wr(gb, HL, A);
  CYC(0x742a, 0x742b); ret_effect(gb);
}
