#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(bipin_showText_subid1To9), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(bipin_showText_subid1To9), (from), (to), true)

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
  BASE(bipin_showText_subid1To9);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+12);
  CYC(b_+6, b_+7); bipin_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); B = 0x43;
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CYC(b_+10, b_+13); showText_hook(gb);
}
