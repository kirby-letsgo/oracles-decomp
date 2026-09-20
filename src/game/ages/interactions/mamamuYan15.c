#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MAMAMU_YAN), bank 0x15.

void mamamuYanRandomizeDogLocation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5de7, 0x5dea); SET_HL(wMamamuDogLocation);

retry_random:
  CALL_C(0x5dea, getRandomNumber_hook, 0x043e, 0x5ded);
  CYC(0x5ded, 0x5def); alu_and(gb, 0x03);
  CYC(0x5def, 0x5df0); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5df0, 0x5df2); goto retry_random; } // jr z
  CYC(0x5df0, 0x5df2);
  CYC(0x5df2, 0x5df3); mem_wr(gb, HL, A);
  RET(0x5df3); return;
}
