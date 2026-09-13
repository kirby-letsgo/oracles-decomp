#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

#define changingFloorData_bank16 0x578a

static void ages_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void loadD6ChangingFloorPatternToBigBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5766, 0x5767); A = B;
  CYC(0x5767, 0x5768); alu_add(gb, A);
  CYC(0x5768, 0x576b); SET_HL(changingFloorData_bank16);
  CYC(0x576b, 0x576c); ages_add_double_index(gb, 0x576c);
  CYC(0x576c, 0x576d); push_effect(gb, HL);
  CYC(0x576d, 0x576e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x576e, 0x576f); D = mem_rd(gb, HL);
  CYC(0x576f, 0x5770); E = A;
  CYC(0x5770, 0x5772); B = 0x41;
  CYC(0x5772, 0x5775); SET_HL(wBigBuffer);
  CALL_C(0x5775, copyMemoryReverse_hook, 0x047f, 0x5778);
  CYC(0x5778, 0x5779); SET_HL(pop_effect(gb));
  CYC(0x5779, 0x577a); SET_HL(HL + 1);
  CYC(0x577a, 0x577b); SET_HL(HL + 1);
  CYC(0x577b, 0x577c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x577c, 0x577d); D = mem_rd(gb, HL);
  CYC(0x577d, 0x577e); E = A;
  CYC(0x577e, 0x5780); B = 0x41;
  CYC(0x5780, 0x5783); SET_HL(wBigBuffer + 0x80);
  CALL_C(0x5783, copyMemoryReverse_hook, 0x047f, 0x5786);
  CYC(0x5786, 0x5788); A = H8(hActiveObject);
  CYC(0x5788, 0x5789); D = A;
  CYC(0x5789, 0x578a); ret_effect(gb);
}
