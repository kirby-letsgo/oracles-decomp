#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/childJabu.s (interactionCodeba /
// INTERAC_CHILD_JABU), bank 0x0b.

// ==================================================================================================
// INTERAC_CHILD_JABU
// ==================================================================================================
void interactionCodeba_hook(GB *gb) {
  BASE(interactionCodeba);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state0; } // jr nz
  CYC(b_+3, b_+5);

  // interactionCodeba@state1
  CALL_C(b_+5, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+8); // SWITCHES THREADS
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+17); SET_BC(0x0e06);
  CALL_C(b_+17, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+20);
  CYC(b_+20, b_+23); SET_HL((SYM(interactionCoded8__subid0Script) + 53)); // mainScripts.childJabuScript
  CALL_C(b_+23, interactionSetScript_hook, SYM(interactionSetScript), b_+26);
  CYC(b_+26, b_+29); TAIL(objectSetVisible82); // jp

state0: // interactionCodeba@state0
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+35); TAIL(interactionRunScript); // jp
}
