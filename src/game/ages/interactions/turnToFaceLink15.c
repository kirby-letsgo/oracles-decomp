#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s, bank 0x15. This seems mostly identical to the
// "turntofacelink" script command, except it uses Link's actual position instead of the
// "hEnemyTargetY/X" variables.

void turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5ca8, objectGetAngleTowardLink_hook, 0x1e9c, 0x5cab);
  CALL_C(0x5cab, convertAngleToDirection_hook, 0x26f9, 0x5cae);
  CYC(0x5cae, 0x5cb1); interactionSetAnimation_hook(gb); return; // jp
}
