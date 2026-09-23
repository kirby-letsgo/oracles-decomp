#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/templeSinkingExplosion.s, bank $09.

static void rst_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Places the explosion at its subid's entry of the stage (wTmpcfc0+$13) table in table_7cd8,
// each coordinate jittered by -2..1.
void s_func_7cb3_hook(GB *gb) {
  BASE(func_7cb3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x13);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x7f);
  CYC(b_+6, b_+9); SET_HL(SYM(table_7cd8));
  CYC(b_+9, b_+10); rst_add_double_index(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+15); E = 0x42; // Interaction.subid
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); rst_add_double_index(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+20); E = 0x4b; // Interaction.yh
  CALL_C(b_+20, s_func_7ccd_hook, SYM(func_7ccd), b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); E = 0x4d; // Interaction.xh
  TAIL_S(func_7ccd);
}

// (de) = a plus a random -2..1.
void s_func_7ccd_hook(GB *gb) {
  BASE(func_7ccd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getRandomNumber_hook, SYM(getRandomNumber), b_+4);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+8); alu_sub(gb, 0x02);
  CYC(b_+8, b_+9); alu_add(gb, B);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}
