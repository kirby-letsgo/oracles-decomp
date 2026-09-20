#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(getObjectDataAddress), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(getObjectDataAddress), (from), (to), true)

void getObjectDataAddress_hook(GB *gb);

static void getObjectDataAddress_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void getObjectDataAddress_hook(GB *gb) {
  BASE(getObjectDataAddress);
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+6); SET_HL(SYM(objectDataGroupTable));
  CYC(b_+6, b_+7); getObjectDataAddress_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); A = mem_rd(gb, wActiveRoom);
  CYC(b_+13, b_+14); E = A;
  CYC(b_+14, b_+16); D = 0x00;
  CYC(b_+16, b_+17); alu_add_hl(gb, DE);
  CYC(b_+17, b_+18); alu_add_hl(gb, DE);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); D = mem_rd(gb, HL);
  CYC(b_+20, b_+21); E = A;
  RET(b_+21);
}
