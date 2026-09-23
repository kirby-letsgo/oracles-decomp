#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_ZELDA), bank 0x15.

// var3f = 1 when Link's health is full.
void s_zelda_checkIfLinkFullyHealed_hook(GB *gb) {
  BASE(zelda_checkIfLinkFullyHealed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wLinkMaxHealth);
  CYC(b_+3, b_+6); A = mem_rd(gb, wLinkHealth);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = 0x7f; // Interaction.var3f
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

void s_zelda_createExclamationMark_hook(GB *gb) {
  BASE(zelda_createExclamationMark);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xf8;
  CYC(b_+2, b_+4); C = 0x10;
  CYC(b_+4, b_+6); A = 0x28;
  CYC(b_+6, b_+9);
  TAIL(objectCreateExclamationMark);
}

void s_resetBit5ofRoomFlags_hook(GB *gb) {
  BASE(resetBit5ofRoomFlags);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0xdf);
  RET(b_+5); return;
}
