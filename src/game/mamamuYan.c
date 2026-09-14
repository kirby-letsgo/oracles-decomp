#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static void mamamuYan_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void mamamuYan_initialize(GB *gb, uint16_t sp0_) {
  CALL_C(0x66d3, interactionInitGraphics_hook, 0x15fb, 0x66d6);
  CYC(0x66d6, 0x66d8); A = 0x0b;
  CALL_C(0x66d8, interactionSetHighTextIndex_hook, 0x253b, 0x66db);
  CYC(0x66db, 0x66dd); E = 0x42;
  CYC(0x66dd, 0x66de); A = mem_rd(gb, DE);
  CYC(0x66de, 0x66e1); SET_HL(0x66eb);
  CYC(0x66e1, 0x66e2); mamamuYan_add_double_index(gb, 0x66e2);
  CYC(0x66e2, 0x66e3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x66e3, 0x66e4); H = mem_rd(gb, HL);
  CYC(0x66e4, 0x66e5); L = A;
  CALL_C(0x66e5, interactionSetScript_hook, 0x2544, 0x66e8);
  CYC(0x66e8, 0x66eb); interactionIncState_hook(gb);
}

void interactionCode53_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66bc, checkInteractionState_hook, 0x23fe, 0x66bf);
  if (F & FZ) {
    CYC(0x66bf, 0x66c1);
    CYC(0x66c1, 0x66c4); push_effect(gb, 0x66c4); mamamuYan_initialize(gb, sp0_);
  } else CYCT(0x66bf, 0x66c1);
  CALL_C(0x66c4, interactionRunScript_hook, 0x2552, 0x66c7);
  if (F & FC) { CYCT(0x66c7, 0x66ca); interactionDelete_hook(gb); return; }
  CYC(0x66c7, 0x66ca);
  CYC(0x66ca, 0x66cd); npcFaceLinkAndAnimate_hook(gb);
}
