#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t past_old_lady_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void past_old_lady_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void past_old_lady_init_graphics_and_inc_state(GB *gb, uint16_t sp0_) {
  CALL_C(0x568f, interactionInitGraphics_hook, 0x15fb, 0x5692);
  CALL_C(0x5692, objectMarkSolidPosition_hook, 0x24f0, 0x5695);
  CYC(0x5695, 0x5698); interactionIncState_hook(gb);
}

static void past_old_lady_init_graphics_text_and_script(GB *gb, uint16_t sp0_) {
  CALL_C(0x5698, interactionInitGraphics_hook, 0x15fb, 0x569b);
  CALL_C(0x569b, objectMarkSolidPosition_hook, 0x24f0, 0x569e);
  CYC(0x569e, 0x56a0); A = 0x18;
  CALL_C(0x56a0, interactionSetHighTextIndex_hook, 0x253b, 0x56a3);
  CYC(0x56a3, 0x56a5); E = 0x42;
  CYC(0x56a5, 0x56a6); A = mem_rd(gb, DE);
  CYC(0x56a6, 0x56a9); SET_HL(0x56b3);
  CYC(0x56a9, 0x56aa); past_old_lady_add_double_index(gb, 0x56aa);
  CYC(0x56aa, 0x56ab); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x56ab, 0x56ac); H = mem_rd(gb, HL);
  CYC(0x56ac, 0x56ad); L = A;
  CALL_C(0x56ad, interactionSetScript_hook, 0x2544, 0x56b0);
  CYC(0x56b0, 0x56b3); interactionIncState_hook(gb);
}

void interactionCode45_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5646, 0x5648); E = 0x42;
  CYC(0x5648, 0x5649); A = mem_rd(gb, DE);
  CYC(0x5649, 0x564a); push_effect(gb, 0x564a);
  switch (past_old_lady_jump_table(gb)) {
    case 0x564e: goto subid0;
    case 0x5664: goto subid1;
    default: HANDOFF(HL);
  }

subid0:
  CALL_C(0x564e, checkInteractionState_hook, 0x23fe, 0x5651);
  if (!(F & FZ)) { CYCT(0x5651, 0x5653); goto run0; }
  CYC(0x5651, 0x5653);
  CYC(0x5653, 0x5655); A = 0x14;
  CALL_C(0x5655, checkGlobalFlag_hook, 0x31f3, 0x5658);
  if (!(F & FZ)) { CYCT(0x5658, 0x565b); interactionDelete_hook(gb); return; }
  CYC(0x5658, 0x565b);
  CYC(0x565b, 0x565e); push_effect(gb, 0x565e);
  past_old_lady_init_graphics_text_and_script(gb, sp0_);

run0:
  CALL_C(0x565e, interactionRunScript_hook, 0x2552, 0x5661);
  CYC(0x5661, 0x5664); interactionAnimateAsNpc_hook(gb);
  return;

subid1:
  CALL_C(0x5664, checkInteractionState_hook, 0x23fe, 0x5667);
  if (!(F & FZ)) { CYCT(0x5667, 0x5669); goto run1; }
  CYC(0x5667, 0x5669);
  CYC(0x5669, 0x566c); SET_HL(0x5559);
  CYC(0x566c, 0x566e); E = 0x09;
  CALL_C(0x566e, interBankCall_hook, 0x008a, 0x5671);
  CYC(0x5671, 0x5672); A = B;
  CYC(0x5672, 0x5674); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(0x5674, 0x5677); interactionDelete_hook(gb); return; }
  CYC(0x5674, 0x5677);
  CYC(0x5677, 0x567a); SET_HL(0x56b7);
  CYC(0x567a, 0x567b); past_old_lady_add_double_index(gb, 0x567b);
  CYC(0x567b, 0x567c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x567c, 0x567d); H = mem_rd(gb, HL);
  CYC(0x567d, 0x567e); L = A;
  CALL_C(0x567e, interactionSetScript_hook, 0x2544, 0x5681);
  CYC(0x5681, 0x5683); A = 0x18;
  CALL_C(0x5683, interactionSetHighTextIndex_hook, 0x253b, 0x5686);
  CYC(0x5686, 0x5689); push_effect(gb, 0x5689);
  past_old_lady_init_graphics_and_inc_state(gb, sp0_);

run1:
  CALL_C(0x5689, interactionRunScript_hook, 0x2552, 0x568c);
  CYC(0x568c, 0x568f); interactionAnimateAsNpc_hook(gb);
}
