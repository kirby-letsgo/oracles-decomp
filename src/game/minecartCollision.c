#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemCode1dPost_hook(GB *gb) {
  CYC(0x5e6a, 0x5e6d); SET_HL(w1Companion_id);
  CYC(0x5e6d, 0x5e6e); A = mem_rd(gb, HL);
  CYC(0x5e6e, 0x5e70); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x5e70, 0x5e73);
    objectTakePosition_hook(gb);
    return;
  }
  CYC(0x5e70, 0x5e73);
  CYC(0x5e73, 0x5e76);
  itemDelete_hook(gb);
}
