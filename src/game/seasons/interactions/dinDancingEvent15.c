#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DIN_DANCING_EVENT), bank 0x15.

// Animation 9 + Link's direction with the vertical axis flipped (Din spins to face him).
void s_dinDancing_spinLink_hook(GB *gb) {
  BASE(dinDancing_spinLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_xor(gb, 0x02);
  CYC(b_+6, b_+8); alu_add(gb, 0x09);
  CYC(b_+8, b_+11);
  TAIL(interactionSetAnimation);
}

// textID = TX_0c00 + a, plus $0a in a linked game.
void s_dinDancingEvent_setTextAdd_0a_ifLinked_hook(GB *gb) {
  BASE(dinDancingEvent_setTextAdd_0a_ifLinked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); C = 0x00;
  CALL_C(b_+3, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto set; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); C = 0x0a;
set:
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+12); alu_add(gb, C);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x72; // Interaction.textID
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0c);
  RET(b_+18); return;
}
