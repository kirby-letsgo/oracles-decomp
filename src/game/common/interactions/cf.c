#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodecf);
  CYC(b_+14, b_+16); E = 0x42;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); SET_HL(b_+32);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  cf_add_double_index(gb);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); E = 0x4b;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+32); objectSetVisible82_hook(gb);
}

void interactionCodecf_hook(GB *gb) {
  BASE(interactionCodecf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (cf_jump_table(gb));
    if (jt_ == b_+8) {
      CYC(b_+8, b_+10); A = 1;
      CYC(b_+10, b_+11); mem_wr(gb, DE, A);
      CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
      interactionCodecf__afterCall4b19_hook(gb);
      return;
    }
    else if (jt_ == b_+38) {
      CYC(b_+38, b_+41); interactionAnimate_hook(gb);
      return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
