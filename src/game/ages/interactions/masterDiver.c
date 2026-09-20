#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodecd);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+30, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+33);
  CALL_C(b_+33, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+36);
  CYC(b_+36, b_+39); interactionIncState_hook(gb); return; // jp
}

// 0b:7840, unused (no callers).
void interactionCodecd_func_7840_hook(GB *gb) {
  BASE(interactionCodecd);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+39, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+42);
  CALL_C(b_+42, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+45);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+51); SET_HL(b_+61); // @scriptTable (empty)
  CYC(b_+51, b_+52); interactionCodecd_addDoubleIndex(gb, b_+52);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+53, b_+54); H = mem_rd(gb, HL);
  CYC(b_+54, b_+55); L = A;
  CALL_C(b_+55, interactionSetScript_hook, SYM(interactionSetScript), b_+58);
  CYC(b_+58, b_+61); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_MASTER_DIVER
//
// Variables:
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCodecd_hook(GB *gb) {
  BASE(interactionCodecd);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interactionCodecd@state0 (0x781e): reached solely by fallthrough, never a jump target.
  CALL_C(b_+5, interactionCodecd_initialize_hook, b_+30, b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x03); // DIVER_SECRET & 0x0f
  CYC(b_+12, b_+15); SET_HL((SYM(interactionCoded8__subid4Script) + 93)); // mainScripts.linkedGameNpcScript
  CALL_C(b_+15, interactionSetScript_hook, SYM(interactionSetScript), b_+18);
  CALL_C(b_+18, interactionRunScript_hook, SYM(interactionRunScript), b_+21);

state1:
  CALL_C(b_+21, interactionRunScript_hook, SYM(interactionRunScript), b_+24);
  if (F & FC) { CYCT(b_+24, b_+27); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30); interactionAnimateAsNpc_hook(gb); return; // jp
}
