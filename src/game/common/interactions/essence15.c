#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void essence_createEnergySwirl_hook(GB *gb);
void essence_stopEnergySwirl_hook(GB *gb);

void essence_createEnergySwirl_hook(GB *gb) {
  BASE(essence_createEnergySwirl);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetPosition_hook, SYM(objectGetPosition), b_+3);
  CYC(b_+3, b_+5); A = 0xff;
  CYC(b_+5, b_+8); TAIL(createEnergySwirlGoingIn);
}

void essence_stopEnergySwirl_hook(GB *gb) {
  BASE(essence_stopEnergySwirl);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDeleteEnergyBeads, A);
  RET(b_+5);
}
