#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void greatFairy_checkScreenIsScrolling_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b14, 0x7b17); A = mem_rd(gb, wScrollMode);
  CYC(0x7b17, 0x7b19); alu_and(gb, 0x01);
  CALL_C(0x7b19, writeFlagsTocddb_hook, 0x5118, 0x7b1c);
  CYC(0x7b1c, 0x7b1d); alu_cpl(gb);
  CYC(0x7b1d, 0x7b20); mem_wr(gb, wcddb, A);
  CYC(0x7b20, 0x7b21); ret_effect(gb);
}
