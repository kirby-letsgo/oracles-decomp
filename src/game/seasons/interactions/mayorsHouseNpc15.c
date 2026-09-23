#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MAYORS_HOUSE_NPC, INTERAC_GORON),
// bank 0x15.

// The next ring box capacity (3 or 5) as text.
void s_getNextRingboxLevel_hook(GB *gb) {
  BASE(getNextRingboxLevel);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wRingBoxLevel);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+6); C = 0x03;
  if (F & FZ) { CYCT(b_+6, b_+8); goto text; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); C = 0x05;
text:
  CYC(b_+10, b_+12); B = 0x00;
  CYC(b_+12, b_+15); SET_HL(wTextNumberSubstitution);
  CYC(b_+15, b_+16); mem_wr(gb, HL, C);
  CYC(b_+16, b_+17); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, B);
  RET(b_+18); return;
}
