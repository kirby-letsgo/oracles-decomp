#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DIN), bank 0x15.

void s_din_createExclamationMark_hook(GB *gb) {
  BASE(din_createExclamationMark);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf300);
  CYC(b_+3, b_+6);
  TAIL(objectCreateExclamationMark);
}
