#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void possessedNayru_moveLinkForward_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d27, 0x6d29); A = 0x0b;
  CYC(0x6d29, 0x6d2c); mem_wr(gb, 0xcc4f, A);
  CYC(0x6d2c, 0x6d2e); A = 0x08;
  CYC(0x6d2e, 0x6d31); mem_wr(gb, 0xcc51, A);
  CYC(0x6d31, 0x6d34); SET_HL(0xd008);
  CYC(0x6d34, 0x6d35); alu_xor(gb, A);
  CYC(0x6d35, 0x6d36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d36, 0x6d37); mem_wr(gb, HL, A);
  CYC(0x6d37, 0x6d38); ret_effect(gb);
}

void possessedNayru_makeExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d38, 0x6d3a); A = 0xf0;
  CALL_C(0x6d3a, playSound_b00_hook, 0x0c98, 0x6d3d);
  CYC(0x6d3d, 0x6d3f); A = 0x18;
  CYC(0x6d3f, 0x6d42); SET_BC(0xf408);
  CYC(0x6d42, 0x6d45); objectCreateExclamationMark_hook(gb);
}
