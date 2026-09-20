#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodebb), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodebb), (from), (to), true)

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
  BASE(interactionCodebb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+14, b_+17); SET_HL((SYM(interactionCoded8__subid0Script) + 58));
  CALL_C(b_+17, interactionSetScript_hook, SYM(interactionSetScript), b_+20);
  CYC(b_+20, b_+23); objectSetVisible82_hook(gb);
}

void interactionCodebb__state0_hook(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  interactionCodebb__afterCall6dd1_hook(gb);
}

void interactionCodebb__state1_hook(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+23, b_+25); A = 0x1a;
  CALL_C(b_+25, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_xor(gb, 0x80);
  CYC(b_+31, b_+32); E = L;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CALL_C(b_+33, interactionRunScript_hook, SYM(interactionRunScript), b_+36);
  if (!(F & FC)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, SYM(interactionCodebc)); interactionDelete_hook(gb);
}

void interactionCodebb_hook(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (human_veran_jump_table(gb));
    if (jt_ == b_+8) { interactionCodebb__state0_hook(gb); return; }
    else if (jt_ == b_+23) { interactionCodebb__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
