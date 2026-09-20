#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(movingPlatform_scriptTable), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(movingPlatform_scriptTable), (from), (to), true)

void essence_createEnergySwirl_hook(GB *gb);
void essence_stopEnergySwirl_hook(GB *gb);

void essence_createEnergySwirl_hook(GB *gb) {
  BASE(movingPlatform_scriptTable);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+138, objectGetPosition_hook, SYM(objectGetPosition), b_+141);
  CYC(b_+141, b_+143); A = 0xff;
  CYC(b_+143, SYM(essence_stopEnergySwirl)); createEnergySwirlGoingIn_hook(gb);
}

void essence_stopEnergySwirl_hook(GB *gb) {
  BASE(essence_stopEnergySwirl);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDeleteEnergyBeads, A);
  RET(b_+5);
}
