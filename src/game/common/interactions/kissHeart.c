#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t kiss_heart_jump_table(GB *gb) {
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

void interactionCodeb7__afterCall48b5_hook(GB *gb) {
  BASE(interactionCodeb7);
  CYC(b_+14, b_+17); objectSetVisible82_hook(gb);
}

void interactionCodeb7_hook(GB *gb) {
  BASE(interactionCodeb7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (kiss_heart_jump_table(gb));
    if (jt_ == b_+8) {
      CYC(b_+8, b_+10); A = 1;
      CYC(b_+10, b_+11); mem_wr(gb, DE, A);
      CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
      interactionCodeb7__afterCall48b5_hook(gb);
      return;
    }
    else if (jt_ == SYM(interactionAnimate)) {
      interactionAnimate_hook(gb);
      return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
