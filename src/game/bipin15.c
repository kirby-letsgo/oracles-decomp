#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void bipin_showText_subid1To9_hook(GB *gb);

static void bipin_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void bipin_showText_subid1To9_hook(GB *gb) {
  CYC(0x4fb1, 0x4fb3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4fb3, 0x4fb4); A = mem_rd(gb, DE);
  CYC(0x4fb4, 0x4fb7); SET_HL(0x4fbd);
  CYC(0x4fb7, 0x4fb8); bipin_addAToHl_from_rst(gb, 0x4fb8);
  CYC(0x4fb8, 0x4fba); B = 0x43;
  CYC(0x4fba, 0x4fbb); C = mem_rd(gb, HL);
  CYC(0x4fbb, 0x4fbe); showText_hook(gb);
}
