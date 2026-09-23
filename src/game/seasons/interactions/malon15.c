#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MALON), bank 0x15.

// var3c = 1 once Talon is back (group 5 room $b6, bit 6).
void s_checkTalonReturned_hook(GB *gb) {
  BASE(checkTalonReturned);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CYC(b_+2, b_+4); B = 0xb6; // ROOM_SEASONS_5b6
  CALL_C(b_+4, getRoomFlags_hook, SYM(getRoomFlags), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x40);
  CYC(b_+9, b_+11); A = 0x01;
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto store; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
store:
  CYC(b_+14, b_+16); E = 0x7c; // Interaction.var3c
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
}
