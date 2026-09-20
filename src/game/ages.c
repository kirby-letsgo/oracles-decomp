#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(loadD6ChangingFloorPatternToBigBuffer), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(loadD6ChangingFloorPatternToBigBuffer), (from), (to), true)

#define changingFloorData_bank16 SYM(loadD6ChangingFloorPatternToBigBuffer__changingFloorData)

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
  BASE(loadD6ChangingFloorPatternToBigBuffer);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+5); SET_HL(changingFloorData_bank16);
  CYC(b_+5, b_+6); ages_add_double_index(gb, b_+6);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); D = mem_rd(gb, HL);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+12); B = 0x41;
  CYC(b_+12, b_+15); SET_HL(wBigBuffer);
  CALL_C(b_+15, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+18);
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, b_+20); SET_HL(HL + 1);
  CYC(b_+20, b_+21); SET_HL(HL + 1);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); D = mem_rd(gb, HL);
  CYC(b_+23, b_+24); E = A;
  CYC(b_+24, b_+26); B = 0x41;
  CYC(b_+26, b_+29); SET_HL(wBigBuffer + 0x80);
  CALL_C(b_+29, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+32);
  CYC(b_+32, b_+34); A = H8(hActiveObject);
  CYC(b_+34, b_+35); D = A;
  CYC(b_+35, b_+36); ret_effect(gb);
}
