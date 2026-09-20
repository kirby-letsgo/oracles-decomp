#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define faroresMemoryScript_bank0c SYM(faroresMemoryScript)

// INTERAC_FARORES_MEMORY: Farore's secret book; only exists once the game has been finished
// or in a linked game.
void interactionCode1c_hook(GB *gb) {
  BASE(interactionCode1c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6); interactionRunScript_hook(gb); return;
  }
  CYC(b_+3, b_+6);

  CYC(b_+6, b_+8); A = GV(0x14, 0x28); // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+8, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+11);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13); goto init;
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+16);
  if (F & FZ) {
    CYCT(b_+16, b_+19); interactionDelete_hook(gb); return;
  }
  CYC(b_+16, b_+19);

init:
  CALL_C(b_+19, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+22);
  CALL_C(b_+22, objectSetVisible83_hook, SYM(objectSetVisible83), b_+25);
  CYC(b_+25, b_+28); SET_HL(faroresMemoryScript_bank0c);
  CALL_C(b_+28, interactionSetScript_hook, SYM(interactionSetScript), b_+31);
  CYC(b_+31, b_+34); interactionIncState_hook(gb);
}
