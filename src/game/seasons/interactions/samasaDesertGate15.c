#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SAMASA_DESERT_GATE), bank 0x15.

void s_samasaDesertGate_createNext2Puffs_hook(GB *gb) {
  BASE(samasaDesertGate_createNext2Puffs);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_samasaDesertGate_createNextPuff_hook, SYM(samasaDesertGate_createNextPuff), b_+3);
  TAIL_S(samasaDesertGate_createNextPuff);
}

// A puff at the next (var3e) entry of samasaDesertGate_puffLocations.
void s_samasaDesertGate_createNextPuff_hook(GB *gb) {
  BASE(samasaDesertGate_createNextPuff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7e; // Interaction.var3e
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+8); SET_BC(SYM(samasaDesertGate_puffLocations));
  CALL_C(b_+8, addDoubleIndexToBc_hook, SYM(addDoubleIndexToBc), b_+11);
  CALL_C(b_+11, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+17, b_+19); L = 0x4b; // Interaction.yh
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); SET_BC(BC + 1);
  CYC(b_+22, b_+24); L = 0x4d; // Interaction.xh
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  RET(b_+26); return;
}
