#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/88.s, bank $09.

// Stores the script pointer hl in var18/19 and loads counter1/counter2 from its first two
// bytes; a first byte of $ff hands over to seasonsFunc_09_7f17.
void s_seasonsFunc_09_7f01_hook(GB *gb) {
  BASE(seasonsFunc_09_7f01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x58; // Interaction.var18
  CYC(b_+2, b_+3); A = H;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = L;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); TAIL_SG(seasonsFunc_09_7f17); }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = 0x46; // Interaction.counter1
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); E = 0x70; // Interaction.var30
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}
