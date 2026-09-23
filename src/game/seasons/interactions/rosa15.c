#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_ROSA), bank 0x15.

void s_rosa_tradeRibbon_hook(GB *gb) {
  BASE(rosa_tradeRibbon);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x77; // Interaction.var37
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+6); A = 0x46; // TREASURE_RIBBON
  CYC(b_+6, b_+9);
  TAIL(loseTreasure);
}

// Rosa becomes subid 1 in state 0 and the date music starts.
void s_rosa_startDate_hook(GB *gb) {
  BASE(rosa_startDate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x42; // Interaction.subid
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+7); L = 0x44; // Interaction.state
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); A = 0x27; // MUS_ROSA_DATE
  CYC(b_+12, b_+15); mem_wr(gb, wActiveMusic, A);
  CYC(b_+15, b_+18);
  TAIL(playSound_b00);
}
