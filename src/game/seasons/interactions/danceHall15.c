#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DANCE_HALL_MINIGAME), bank 0x15.

// seasonsFunc_15_5d20 from its +24 (the shared tail): wTmpcfc0+$14 = a, +$11 = 9, +$1a bumped.
static void dance_set_round(GB *gb) {
  BASE(seasonsFunc_15_5d20);
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcfc0 + 0x14, A);
  CYC(b_+27, b_+29); A = 0x09;
  CYC(b_+29, b_+32); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+32, b_+35); SET_HL(wTmpcfc0 + 0x1a);
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+36); return;
}

void s_seasonsFunc_15_5d20_hook(GB *gb) {
  BASE(seasonsFunc_15_5d20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wTmpcfc0 + 0x12, A);
  CYC(b_+5, b_+7); A = 0x04;
  CYCT(b_+7, b_+9); dance_set_round(gb); return;
}

void s_seasonsFunc_15_5d29_hook(GB *gb) {
  BASE(seasonsFunc_15_5d29);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wTmpcfc0 + 0x12, A);
  CYC(b_+5, b_+7); A = 0x04;
  CYCT(b_+7, b_+9); dance_set_round(gb); return;
}

void s_seasonsFunc_15_5d32_hook(GB *gb) {
  BASE(seasonsFunc_15_5d32);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CYCT(b_+2, b_+4); dance_set_round(gb); return;
}

// var14/15 = $80/$fe, var0e = 1.
void s_seasonsFunc_15_5d45_hook(GB *gb) {
  BASE(seasonsFunc_15_5d45);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x54; // Interaction.speedZ
  CYC(b_+2, b_+4); A = 0x80;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0xfe;
  CYC(b_+7, b_+8); E = alu_inc8(gb, E);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); E = 0x4e; // Interaction.z
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return;
}
