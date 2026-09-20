#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(mamamuYanRandomizeDogLocation), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(mamamuYanRandomizeDogLocation), (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MAMAMU_YAN), bank 0x15.

void mamamuYanRandomizeDogLocation_hook(GB *gb) {
  BASE(mamamuYanRandomizeDogLocation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wMamamuDogLocation);

retry_random:
  CALL_C(b_+3, getRandomNumber_hook, SYM(getRandomNumber), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+11); goto retry_random; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  RET(b_+12); return;
}
