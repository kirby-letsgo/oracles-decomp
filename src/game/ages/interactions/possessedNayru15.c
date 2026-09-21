#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void possessedNayru_moveLinkForward_hook(GB *gb) {
  BASE(possessedNayru_moveLinkForward);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkForceState, A);
  CYC(b_+5, b_+7); A = 0x08;
  CYC(b_+7, b_+10); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+10, b_+13); SET_HL(w1Link_direction);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void possessedNayru_makeExclamationMark_hook(GB *gb) {
  BASE(possessedNayru_makeExclamationMark);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xf0;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+7); A = 0x18;
  CYC(b_+7, b_+10); SET_BC(0xf408);
  CYC(b_+10, b_+13); TAIL(objectCreateExclamationMark);
}
