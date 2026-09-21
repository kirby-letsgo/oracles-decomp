#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void cheval_setTalkedGlobalflag_hook(GB *gb) {
  BASE(cheval_setTalkedGlobalflag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x43;
  CYC(b_+2, b_+5); TAIL(setGlobalFlag);
}
