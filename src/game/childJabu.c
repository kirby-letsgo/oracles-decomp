#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/childJabu.s (interactionCodeba /
// INTERAC_CHILD_JABU), bank 0x0b.

// ==================================================================================================
// INTERAC_CHILD_JABU
// ==================================================================================================
void interactionCodeba_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6da0, checkInteractionState_hook, 0x23fe, 0x6da3);
  if (!(F & FZ)) { CYCT(0x6da3, 0x6da5); goto state0; } // jr nz
  CYC(0x6da3, 0x6da5);

  // interactionCodeba@state1
  CALL_C(0x6da5, interactionInitGraphics_hook, 0x15fb, 0x6da8); // SWITCHES THREADS
  CALL_C(0x6da8, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6dab);
  CALL_C(0x6dab, interactionIncState_hook, 0x23e0, 0x6dae);
  CYC(0x6dae, 0x6db1); SET_BC(0x0e06);
  CALL_C(0x6db1, objectSetCollideRadii_hook, 0x24a9, 0x6db4);
  CYC(0x6db4, 0x6db7); SET_HL(0x7d8b); // mainScripts.childJabuScript
  CALL_C(0x6db7, interactionSetScript_hook, 0x2544, 0x6dba);
  CYC(0x6dba, 0x6dbd); objectSetVisible82_hook(gb); return; // jp

state0: // interactionCodeba@state0
  CALL_C(0x6dbd, interactionAnimateAsNpc_hook, 0x26db, 0x6dc0);
  CYC(0x6dc0, 0x6dc3); interactionRunScript_hook(gb); return; // jp
}
