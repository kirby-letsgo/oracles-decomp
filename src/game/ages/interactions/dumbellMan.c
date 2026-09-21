#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void dumbbellMan_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void dumbbellMan_initialize(GB *gb, uint16_t sp0_) {
  BASE(interactionCode51);
  CALL_C(b_+26, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+29);
  CYC(b_+29, b_+31); A = 0x0b;
  CALL_C(b_+31, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+34);
  CYC(b_+34, b_+36); E = 0x42;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+40); SET_HL(b_+50);
  CYC(b_+40, b_+41); dumbbellMan_add_double_index(gb, b_+41);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); H = mem_rd(gb, HL);
  CYC(b_+43, b_+44); L = A;
  CALL_C(b_+44, interactionSetScript_hook, SYM(interactionSetScript), b_+47);
  CYC(b_+47, b_+50); interactionIncState_hook(gb);
}

void interactionCode51_hook(GB *gb) {
  BASE(interactionCode51);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+8); push_effect(gb, b_+8); dumbbellMan_initialize(gb, sp0_);
    CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  } else CYCT(b_+3, b_+5);
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  if (F & FC) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20); TAIL(interactionAnimateAsNpc);
}
