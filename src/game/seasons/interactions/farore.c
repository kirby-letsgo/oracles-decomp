#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/farore.s.
// INTERAC_FARORE

// Indirect caller for INTERAC_FARORE_MAKECHEST
void s_interactionCode11_caller_hook(GB *gb) {
  BASE(interactionCode11_caller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(interactionCode11));
  CYC(b_+3, b_+5); E = 0x3f;
  CYC(b_+5, b_+8);
  TAIL(interBankCall);
}

