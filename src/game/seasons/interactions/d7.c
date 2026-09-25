#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/d7.s.
// INTERAC_STUB_d7

// INTERAC_STUB_d7
void s_interactionCoded7_hook(GB *gb) {
  BASE(interactionCoded7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(interactionDelete);
}

