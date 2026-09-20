#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void essence_createEnergySwirl_hook(GB *gb);
void essence_stopEnergySwirl_hook(GB *gb);

void essence_createEnergySwirl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4248, objectGetPosition_hook, 0x208a, 0x424b);
  CYC(0x424b, 0x424d); A = 0xff;
  CYC(0x424d, 0x4250); createEnergySwirlGoingIn_hook(gb);
}

void essence_stopEnergySwirl_hook(GB *gb) {
  CYC(0x4250, 0x4252); A = 0x01;
  CYC(0x4252, 0x4255); mem_wr(gb, 0xcd2d, A);
  RET(0x4255);
}
