#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t cf_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 4, 6, true);
  }
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void cf_add_double_index(GB *gb) {
  burn_rom(gb, 0, 0x18, 0x19, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x19, 0x1a, false); C = A;
  burn_rom(gb, 0, 0x1a, 0x1c, false); B = 0;
  burn_rom(gb, 0, 0x1c, 0x1d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1d, 0x1e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1e, 0x1f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x1f, 0x20, false); ret_effect(gb);
}

void interactionCodecf__afterCall4b19_hook(GB *gb) {
  CYC(0x4b19, 0x4b1b); E = 0x42;
  CYC(0x4b1b, 0x4b1c); A = mem_rd(gb, DE);
  CYC(0x4b1c, 0x4b1f); SET_HL(0x4b2b);
  CYC(0x4b1f, 0x4b20); push_effect(gb, 0x4b20);
  cf_add_double_index(gb);
  CYC(0x4b20, 0x4b21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4b21, 0x4b23); E = 0x4b;
  CYC(0x4b23, 0x4b24); mem_wr(gb, DE, A);
  CYC(0x4b24, 0x4b25); E = alu_inc8(gb, E);
  CYC(0x4b25, 0x4b26); E = alu_inc8(gb, E);
  CYC(0x4b26, 0x4b27); A = mem_rd(gb, HL);
  CYC(0x4b27, 0x4b28); mem_wr(gb, DE, A);
  CYC(0x4b28, 0x4b2b); objectSetVisible82_hook(gb);
}

void interactionCodecf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b0b, 0x4b0d); E = 0x44;
  CYC(0x4b0d, 0x4b0e); A = mem_rd(gb, DE);
  CYC(0x4b0e, 0x4b0f); push_effect(gb, 0x4b0f);
  switch (cf_jump_table(gb)) {
    case 0x4b13:
      CYC(0x4b13, 0x4b15); A = 1;
      CYC(0x4b15, 0x4b16); mem_wr(gb, DE, A);
      CALL_C(0x4b16, interactionInitGraphics_hook, 0x15fb, 0x4b19);
      interactionCodecf__afterCall4b19_hook(gb);
      return;
    case 0x4b31:
      CYC(0x4b31, 0x4b34); interactionAnimate_hook(gb);
      return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
