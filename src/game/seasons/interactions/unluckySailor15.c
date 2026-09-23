#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_UNLUCKY_SAILOR), bank 0x15.

// var39 = 1 when Link has exactly 777 ore chunks.
void s_unluckySailor_checkHave777OreChunks_hook(GB *gb) {
  BASE(unluckySailor_checkHave777OreChunks);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = 0x79; // Interaction.var39
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7); SET_HL(wNumOreChunks);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+10); alu_cp(gb, 0x77);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return;
}

// Bomb capacity and count both up by $20 (BCD).
void s_unluckySailor_increaseBombCapacityAndCount_hook(GB *gb) {
  BASE(unluckySailor_increaseBombCapacityAndCount);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMaxBombs);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_add(gb, 0x20);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+11);
  TAIL(setStatusBarNeedsRefreshBit1);
}
