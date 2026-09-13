#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define faroresMemoryScript_bank0c 0x469c

// INTERAC_FARORES_MEMORY: Farore's secret book; only exists once the game has been finished
// or in a linked game.
void interactionCode1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x46c2, checkInteractionState_hook, 0x23fe, 0x46c5);
  if (!(F & FZ)) {
    CYCT(0x46c5, 0x46c8); interactionRunScript_hook(gb); return;
  }
  CYC(0x46c5, 0x46c8);

  CYC(0x46c8, 0x46ca); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x46ca, checkGlobalFlag_hook, 0x31f3, 0x46cd);
  if (!(F & FZ)) {
    CYCT(0x46cd, 0x46cf); goto init;
  }
  CYC(0x46cd, 0x46cf);
  CALL_C(0x46cf, checkIsLinkedGame_hook, 0x1992, 0x46d2);
  if (F & FZ) {
    CYCT(0x46d2, 0x46d5); interactionDelete_hook(gb); return;
  }
  CYC(0x46d2, 0x46d5);

init:
  CALL_C(0x46d5, interactionInitGraphics_hook, 0x15fb, 0x46d8);
  CALL_C(0x46d8, objectSetVisible83_hook, 0x1e72, 0x46db);
  CYC(0x46db, 0x46de); SET_HL(faroresMemoryScript_bank0c);
  CALL_C(0x46de, interactionSetScript_hook, 0x2544, 0x46e1);
  CYC(0x46e1, 0x46e4); interactionIncState_hook(gb);
}
