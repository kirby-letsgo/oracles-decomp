#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_LINKED_GAME_GHINI), bank 0x15.

// var3f = 1 when the selected text option (xor 1) differs from var3c.
void s_seasonsFunc_15_64a0_hook(GB *gb) {
  BASE(seasonsFunc_15_64a0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7c; // Interaction.var3c
  CYC(b_+3, b_+6); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+6, b_+8); alu_xor(gb, 0x01);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); L = 0x7f; // Interaction.var3f
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto differ; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  RET(b_+15); return;
differ:
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01);
  RET(b_+18); return;
}

void s_linkedGhini_clearAllAndSetInvisible_hook(GB *gb) {
  BASE(linkedGhini_clearAllAndSetInvisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_clearAllItemsAndPutLinkOnGround, SYM(clearAllItemsAndPutLinkOnGround), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetInvisible);
}

void s_linkedGhini_setVisible_hook(GB *gb) {
  BASE(linkedGhini_setVisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(objectSetVisible);
}

// Link facing up at ($5c,$50) on the ground, in the forced state.
void s_linkedGhini_forceLinksPositionAndState_hook(GB *gb) {
  BASE(linkedGhini_forceLinksPositionAndState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link_direction);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(b_+8, b_+10); L = 0x0b; // w1Link.yh
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x5c);
  CYC(b_+12, b_+14); L = 0x0d; // w1Link.xh
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x50);
  CYC(b_+16, b_+18); L = 0x0f; // w1Link.zh
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00);
  RET(b_+20); return;
}
