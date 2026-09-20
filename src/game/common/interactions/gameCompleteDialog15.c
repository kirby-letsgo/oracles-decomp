#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void gameCompleteDialog_markGameAsComplete_hook(GB *gb);

void gameCompleteDialog_markGameAsComplete_hook(GB *gb) {
  CYC(0x42fe, 0x42ff); alu_xor(gb, A);
  CYC(0x42ff, 0x4302); mem_wr(gb, 0xc641, A);
  CYC(0x4302, 0x4303); A = alu_inc8(gb, A);
  CYC(0x4303, 0x4306); mem_wr(gb, 0xc614, A);
  CYC(0x4306, 0x4308); A = 0x1c;
  CYC(0x4308, 0x430b); mem_wr(gb, 0xc6e6, A);
  CYC(0x430b, 0x430d); A = 0x8c;
  CYC(0x430d, 0x4310); mem_wr(gb, 0xc6e7, A);
  CYC(0x4310, 0x4312); A = 0x14;
  CYC(0x4312, 0x4315); setGlobalFlag_hook(gb);
}
