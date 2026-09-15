#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t human_veran_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCodebb__afterCall6dd1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dd1, 0x6dd4); SET_HL(0x7d90);
  CALL_C(0x6dd4, interactionSetScript_hook, 0x2544, 0x6dd7);
  CYC(0x6dd7, 0x6dda); objectSetVisible82_hook(gb);
}

void interactionCodebb__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dcb, interactionIncState_hook, 0x23e0, 0x6dce);
  CALL_C(0x6dce, interactionInitGraphics_hook, 0x15fb, 0x6dd1);
  interactionCodebb__afterCall6dd1_hook(gb);
}

void interactionCodebb__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dda, 0x6ddc); A = 0x1a;
  CALL_C(0x6ddc, objectGetRelatedObject1Var_hook, 0x2160, 0x6ddf);
  CYC(0x6ddf, 0x6de0); A = mem_rd(gb, HL);
  CYC(0x6de0, 0x6de2); alu_xor(gb, 0x80);
  CYC(0x6de2, 0x6de3); E = L;
  CYC(0x6de3, 0x6de4); mem_wr(gb, DE, A);
  CALL_C(0x6de4, interactionRunScript_hook, 0x2552, 0x6de7);
  if (!(F & FC)) { CYCT(0x6de7, 0x6de8); ret_effect(gb); return; }
  CYC(0x6de7, 0x6de8);
  CYC(0x6de8, 0x6deb); interactionDelete_hook(gb);
}

void interactionCodebb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dc3, 0x6dc5); E = 0x44;
  CYC(0x6dc5, 0x6dc6); A = mem_rd(gb, DE);
  CYC(0x6dc6, 0x6dc7); push_effect(gb, 0x6dc7);
  switch (human_veran_jump_table(gb)) {
    case 0x6dcb: interactionCodebb__state0_hook(gb); return;
    case 0x6dda: interactionCodebb__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
