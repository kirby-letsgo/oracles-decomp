#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_STEALING_FEATHER), bank 0x15.

// Another stealing-feather object (subid 0) at Link's position.
void s_stealingFeather_spawnSelfWithSubId0_hook(GB *gb) {
  BASE(stealingFeather_spawnSelfWithSubId0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x6e); // INTERAC_STEALING_FEATHER
  CYC(b_+6, b_+8); L = 0x4b; // Interaction.yh
  CYC(b_+8, b_+11); A = mem_rd(gb, w1Link_yh);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+16); A = mem_rd(gb, w1Link_xh);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  RET(b_+17); return;
}

void s_stealingFeather_putLinkOnGround_hook(GB *gb) {
  BASE(stealingFeather_putLinkOnGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6);
  TAIL(putLinkOnGround);
}

// The two strange brothers (Subrosian subids $10 and $11).
void s_stealingFeather_spawnStrangeBrothers_hook(GB *gb) {
  BASE(stealingFeather_spawnStrangeBrothers);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x30a8);
  CYC(b_+3, b_+5); E = 0x10;
  CALL_C(b_+5, s_func_5e82_hook, SYM(func_5e82), b_+8);
  CYC(b_+8, b_+11); SET_BC(0x34b8);
  CYC(b_+11, b_+13); E = 0x11;
  TAIL_S(func_5e82);
}

// A Subrosian with subid e at (b,c).
void s_func_5e82_hook(GB *gb) {
  BASE(func_5e82);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x30); // INTERAC_SUBROSIAN
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, E);
  CYC(b_+8, b_+10); L = 0x4b; // Interaction.yh
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  CYC(b_+11, b_+13); L = 0x4d; // Interaction.xh
  CYC(b_+13, b_+14); mem_wr(gb, HL, C);
  RET(b_+14); return;
}
