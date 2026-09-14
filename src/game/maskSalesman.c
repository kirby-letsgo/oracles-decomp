#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static void mask_salesman_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void mask_salesman_load_script_and_init_graphics(GB *gb, uint16_t sp0_) {
  CALL_C(0x6d28, interactionInitGraphics_hook, 0x15fb, 0x6d2b);
  CYC(0x6d2b, 0x6d2d); E = OBJ_SUBID; CYC(0x6d2d, 0x6d2e); A = mem_rd(gb, DE); CYC(0x6d2e, 0x6d31); SET_HL(0x6d3b);
  CYC(0x6d31, 0x6d32); mask_salesman_add_double_index(gb, 0x6d32); CYC(0x6d32, 0x6d33); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d33, 0x6d34); H = mem_rd(gb, HL); CYC(0x6d34, 0x6d35); L = A; CALL_C(0x6d35, interactionSetScript_hook, 0x2544, 0x6d38);
  CYC(0x6d38, 0x6d3b); interactionIncState_hook(gb);
}

void interactionCode5c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d0e, checkInteractionState_hook, 0x23fe, 0x6d11);
  if (!(F & FZ)) CYCT(0x6d11, 0x6d13); else { CYC(0x6d11, 0x6d13); CYC(0x6d13, 0x6d16); push_effect(gb, 0x6d16); mask_salesman_load_script_and_init_graphics(gb, sp0_); CALL_C(0x6d16, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6d19); }
  CALL_C(0x6d19, interactionRunScript_hook, 0x2552, 0x6d1c);
  if (F & FC) { CYCT(0x6d1c, 0x6d1f); interactionDelete_hook(gb); return; }
  CYC(0x6d1c, 0x6d1f); CYC(0x6d1f, 0x6d22); interactionAnimateAsNpc_hook(gb);
}
