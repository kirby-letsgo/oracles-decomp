#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(moblin_spawnEnemyHere), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(moblin_spawnEnemyHere), (from), (to), true)

void moblin_spawnEnemyHere_hook(GB *gb) {
  BASE(moblin_spawnEnemyHere);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x20);
  CYC(b_+6, SYM(carpenter_buildBridgeColumn)); objectCopyPosition_hook(gb);
}
