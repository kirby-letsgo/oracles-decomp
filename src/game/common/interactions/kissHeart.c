#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  CYC(0x48b5, 0x48b8); objectSetVisible82_hook(gb);
}

void interactionCodeb7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x48a7, 0x48a9); E = 0x44;
  CYC(0x48a9, 0x48aa); A = mem_rd(gb, DE);
  CYC(0x48aa, 0x48ab); push_effect(gb, 0x48ab);
  switch (kiss_heart_jump_table(gb)) {
    case 0x48af:
      CYC(0x48af, 0x48b1); A = 1;
      CYC(0x48b1, 0x48b2); mem_wr(gb, DE, A);
      CALL_C(0x48b2, interactionInitGraphics_hook, 0x15fb, 0x48b5);
      interactionCodeb7__afterCall48b5_hook(gb);
      return;
    case 0x261b:
      interactionAnimate_hook(gb);
      return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
