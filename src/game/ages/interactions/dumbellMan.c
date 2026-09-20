#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

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
  CALL_C(0x65bf, interactionInitGraphics_hook, 0x15fb, 0x65c2);
  CYC(0x65c2, 0x65c4); A = 0x0b;
  CALL_C(0x65c4, interactionSetHighTextIndex_hook, 0x253b, 0x65c7);
  CYC(0x65c7, 0x65c9); E = 0x42;
  CYC(0x65c9, 0x65ca); A = mem_rd(gb, DE);
  CYC(0x65ca, 0x65cd); SET_HL(0x65d7);
  CYC(0x65cd, 0x65ce); dumbbellMan_add_double_index(gb, 0x65ce);
  CYC(0x65ce, 0x65cf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65cf, 0x65d0); H = mem_rd(gb, HL);
  CYC(0x65d0, 0x65d1); L = A;
  CALL_C(0x65d1, interactionSetScript_hook, 0x2544, 0x65d4);
  CYC(0x65d4, 0x65d7); interactionIncState_hook(gb);
}

void interactionCode51_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x65a5, checkInteractionState_hook, 0x23fe, 0x65a8);
  if (F & FZ) {
    CYC(0x65a8, 0x65aa);
    CYC(0x65aa, 0x65ad); push_effect(gb, 0x65ad); dumbbellMan_initialize(gb, sp0_);
    CALL_C(0x65ad, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x65b0);
  } else CYCT(0x65a8, 0x65aa);
  CALL_C(0x65b0, interactionRunScript_hook, 0x2552, 0x65b3);
  if (F & FC) { CYCT(0x65b3, 0x65b6); interactionDelete_hook(gb); return; }
  CYC(0x65b3, 0x65b6);
  CYC(0x65b6, 0x65b9); interactionAnimateAsNpc_hook(gb);
}
