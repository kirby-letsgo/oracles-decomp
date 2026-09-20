#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void moblin_spawnEnemyHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7592, getFreeEnemySlot_hook, 0x2e27, 0x7595);
  if (!(F & FZ)) { RET_TAKEN(0x7595); return; }
  CYC(0x7595, 0x7596);
  CYC(0x7596, 0x7598); mem_wr(gb, HL, 0x20);
  CYC(0x7598, 0x759b); objectCopyPosition_hook(gb);
}
