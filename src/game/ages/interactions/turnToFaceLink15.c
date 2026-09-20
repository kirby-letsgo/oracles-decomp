#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(turnToFaceLink), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(turnToFaceLink), (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s, bank 0x15. This seems mostly identical to the
// "turntofacelink" script command, except it uses Link's actual position instead of the
// "hEnemyTargetY/X" variables.

void turnToFaceLink_hook(GB *gb) {
  BASE(turnToFaceLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CALL_C(b_+3, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+6);
  CYC(b_+6, SYM(ambiFlickerVisibility)); interactionSetAnimation_hook(gb); return; // jp
}
