#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(cheval_setTalkedGlobalflag), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(cheval_setTalkedGlobalflag), (from), (to), true)

void cheval_setTalkedGlobalflag_hook(GB *gb) {
  BASE(cheval_setTalkedGlobalflag);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x43;
  CYC(b_+2, SYM(interaction6b_loadMoblinsAttackingMakuSprout)); setGlobalFlag_hook(gb);
}
