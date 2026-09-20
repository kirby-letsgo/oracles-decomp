#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static void old_zora_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void old_zora_load_script_and_init_graphics(GB *gb, uint16_t sp0_) {
  CALL_C(0x6c3a, interactionInitGraphics_hook, 0x15fb, 0x6c3d);
  CYC(0x6c3d, 0x6c3f); A = 0x0b;
  CALL_C(0x6c3f, interactionSetHighTextIndex_hook, 0x253b, 0x6c42);
  CYC(0x6c42, 0x6c44); E = OBJ_SUBID; CYC(0x6c44, 0x6c45); A = mem_rd(gb, DE);
  CYC(0x6c45, 0x6c48); SET_HL(0x6c52); CYC(0x6c48, 0x6c49); old_zora_add_double_index(gb, 0x6c49);
  CYC(0x6c49, 0x6c4a); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6c4a, 0x6c4b); H = mem_rd(gb, HL); CYC(0x6c4b, 0x6c4c); L = A;
  CALL_C(0x6c4c, interactionSetScript_hook, 0x2544, 0x6c4f); CYC(0x6c4f, 0x6c52); interactionIncState_hook(gb);
}

void interactionCode5a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6c20, checkInteractionState_hook, 0x23fe, 0x6c23);
  if (!(F & FZ)) CYCT(0x6c23, 0x6c25); else { CYC(0x6c23, 0x6c25); CYC(0x6c25, 0x6c28); push_effect(gb, 0x6c28); old_zora_load_script_and_init_graphics(gb, sp0_); CALL_C(0x6c28, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6c2b); }
  CALL_C(0x6c2b, interactionRunScript_hook, 0x2552, 0x6c2e);
  if (F & FC) { CYCT(0x6c2e, 0x6c31); interactionDelete_hook(gb); return; }
  CYC(0x6c2e, 0x6c31); CYC(0x6c31, 0x6c34); interactionAnimateAsNpc_hook(gb);
}
