#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemBounce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6482, 0x6484); A = 0x52;
  CALL_C(0x6484, playSound_b00_hook, 0x0c98, 0x6487);
  CALL_C(0x6487, objectNegateAndHalveSpeedZ_hook, 0x2374, 0x648a);
  if (F & FC) {
    CYCT(0x648a, 0x648b); ret_effect(gb); return;
  }
  CYC(0x648a, 0x648b);
  CYC(0x648b, 0x648d); E = 0x10;
  CYC(0x648d, 0x648e); A = mem_rd(gb, DE);
  CYC(0x648e, 0x648f); E = A;
  CYC(0x648f, 0x6492); SET_HL(0x64d2);
  CALL_C(0x6492, lookupKey_hook, 0x1e06, 0x6495);
  CYC(0x6495, 0x6497); E = 0x10;
  CYC(0x6497, 0x6498); mem_wr(gb, DE, A);
  CYC(0x6498, 0x6499); alu_or(gb, A);
  CYC(0x6499, 0x649a); ret_effect(gb);
}
