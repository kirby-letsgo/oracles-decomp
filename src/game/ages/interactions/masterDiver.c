#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodecd_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:7837, called once from interactionCodecd@state0.
void interactionCodecd_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7837, interactionInitGraphics_hook, 0x15fb, 0x783a);
  CALL_C(0x783a, objectMarkSolidPosition_hook, 0x24f0, 0x783d);
  CYC(0x783d, 0x7840); interactionIncState_hook(gb); return; // jp
}

// 0b:7840, unused (no callers).
void interactionCodecd_func_7840_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7840, interactionInitGraphics_hook, 0x15fb, 0x7843);
  CALL_C(0x7843, objectMarkSolidPosition_hook, 0x24f0, 0x7846);
  CYC(0x7846, 0x7848); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7848, 0x7849); A = mem_rd(gb, DE);
  CYC(0x7849, 0x784c); SET_HL(0x7856); // @scriptTable (empty)
  CYC(0x784c, 0x784d); interactionCodecd_addDoubleIndex(gb, 0x784d);
  CYC(0x784d, 0x784e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x784e, 0x784f); H = mem_rd(gb, HL);
  CYC(0x784f, 0x7850); L = A;
  CALL_C(0x7850, interactionSetScript_hook, 0x2544, 0x7853);
  CYC(0x7853, 0x7856); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_MASTER_DIVER
//
// Variables:
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCodecd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7819, checkInteractionState_hook, 0x23fe, 0x781c);
  if (!(F & FZ)) { CYCT(0x781c, 0x781e); goto state1; } // jr nz
  CYC(0x781c, 0x781e);

  // interactionCodecd@state0 (0x781e): reached solely by fallthrough, never a jump target.
  CALL_C(0x781e, interactionCodecd_initialize_hook, 0x7837, 0x7821);
  CYC(0x7821, 0x7823); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7823, 0x7825); mem_wr(gb, HL, 0x03); // DIVER_SECRET & 0x0f
  CYC(0x7825, 0x7828); SET_HL(0x7ed9); // mainScripts.linkedGameNpcScript
  CALL_C(0x7828, interactionSetScript_hook, 0x2544, 0x782b);
  CALL_C(0x782b, interactionRunScript_hook, 0x2552, 0x782e);

state1:
  CALL_C(0x782e, interactionRunScript_hook, 0x2552, 0x7831);
  if (F & FC) { CYCT(0x7831, 0x7834); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(0x7831, 0x7834);
  CYC(0x7834, 0x7837); interactionAnimateAsNpc_hook(gb); return; // jp
}
