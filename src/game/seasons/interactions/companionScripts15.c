#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_COMPANION_SCRIPTS), bank 0x15.

// The companion jumps (speedZ -$140) into its cutscene state $0b with var03 = 3, OAM flags 9.
void s_seasonsFunc_15_5e91_hook(GB *gb) {
  BASE(seasonsFunc_15_5e91);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Companion_speedZ);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0xc0);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xfe);
  CYC(b_+8, b_+10); L = 0x3f; // w1Companion.var3f
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0b);
  CYC(b_+12, b_+14); L = 0x03; // w1Companion.var03
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x03);
  CYC(b_+16, b_+18); L = 0x1c; // w1Companion.oamFlags
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x09);
  RET(b_+20); return;
}

void s_seasonsFunc_15_5ea6_hook(GB *gb) {
  BASE(seasonsFunc_15_5ea6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Companion_var03);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04);
  CYC(b_+5, b_+7); L = 0x1a; // w1Companion.visible
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xc0);
  CYC(b_+9, b_+11); L = 0x3f; // w1Companion.var3f
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x19);
  RET(b_+13); return;
}

// Link walks left at speed $140; the companion's cutscene state $1d.
void s_seasonsFunc_15_5eb4_hook(GB *gb) {
  BASE(seasonsFunc_15_5eb4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x18; // ANGLE_LEFT
  CYC(b_+2, b_+5); mem_wr(gb, wLinkAngle, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_angle);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = 0x10; // w1Link.speed
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+13, b_+15); A = 0x1d;
  CYC(b_+15, b_+18); mem_wr(gb, w1Companion_var3f, A);
  RET(b_+18); return;
}

// Link and the companion face down; the companion's cutscene state 3 with var03 = 6.
void s_seasonsFunc_15_5ec7_hook(GB *gb) {
  BASE(seasonsFunc_15_5ec7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02; // DIR_DOWN
  CYC(b_+2, b_+5); mem_wr(gb, w1Link_direction, A);
  CYC(b_+5, b_+8); SET_HL(w1Companion_direction);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+13, b_+15); L = 0x03; // w1Companion.var03
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x06);
  CYC(b_+17, b_+19); A = 0x03;
  CYC(b_+19, b_+22); mem_wr(gb, w1Companion_var3f, A);
  RET(b_+22); return;
}

// var3b = 1 when the companion is Moosh.
void s_seasonsFunc_15_5ede_hook(GB *gb) {
  BASE(seasonsFunc_15_5ede);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0d); // SPECIALOBJECT_MOOSH
  CYC(b_+5, b_+7); A = 0x01;
  if (F & FZ) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
store:
  CYC(b_+10, b_+12); E = 0x7b; // Interaction.var3b
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}
