#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void cheval_setTalkedGlobalflag_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b7f, 0x6b81); A = 0x43;
  CYC(0x6b81, 0x6b84); setGlobalFlag_hook(gb);
}
