#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  CYC(0x4315, 0x4318); A = mem_rd(gb, 0xcc2d);
  CYC(0x4318, 0x431b); SET_HL(0x432b);
  CYC(0x431b, 0x431c); getObjectDataAddress_addDoubleIndexToHl_from_rst(gb, 0x431c);
  CYC(0x431c, 0x431d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x431d, 0x431e); H = mem_rd(gb, HL);
  CYC(0x431e, 0x431f); L = A;
  CYC(0x431f, 0x4322); A = mem_rd(gb, 0xcc30);
  CYC(0x4322, 0x4323); E = A;
  CYC(0x4323, 0x4325); D = 0x00;
  CYC(0x4325, 0x4326); alu_add_hl(gb, DE);
  CYC(0x4326, 0x4327); alu_add_hl(gb, DE);
  CYC(0x4327, 0x4328); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4328, 0x4329); D = mem_rd(gb, HL);
  CYC(0x4329, 0x432a); E = A;
  RET(0x432a);
}
