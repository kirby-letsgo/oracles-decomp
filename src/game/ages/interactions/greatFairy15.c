#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void greatFairy_checkScreenIsScrolling_hook(GB *gb) {
  BASE(greatFairy_checkScreenIsScrolling);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CALL_C(b_+5, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+8);
  CYC(b_+8, b_+9); alu_cpl(gb);
  CYC(b_+9, b_+12); mem_wr(gb, wcddb, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}
