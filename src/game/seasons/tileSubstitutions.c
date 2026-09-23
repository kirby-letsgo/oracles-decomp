#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/tileSubstitutions.s, bank $04, the Seasons-only pieces.

// In Subrosia (minimap group 1), object gfx header 6 goes to slot 4.
void s_loadSubrosiaObjectGfxHeader_hook(GB *gb) {
  BASE(loadSubrosiaObjectGfxHeader);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMinimapGroup);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x06;
  CYC(b_+8, b_+11);
  TAIL(loadObjectGfxHeaderToSlot4);
}
