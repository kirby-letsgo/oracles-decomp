#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void pirate_captain_state0_tail_hook(GB *gb) {
  BASE(interactionCodec3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+17, objectSetVisible82_hook, SYM(objectSetVisible82), b_+20);
  CALL_C(b_+20, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+23);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
  } else {
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+28); SET_HL(wTilesetFlags);
    CYC(b_+28, b_+30); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  }
  CYC(b_+30, b_+33); SET_HL((SYM(interactionCoded8__subid1Script) + 24));
  CALL_C(b_+33, interactionSetScript_hook, SYM(interactionSetScript), b_+36);
  CYC(b_+36, b_+39); TAIL(interactionIncState);
}

void interactionCodec3__afterCall7354_hook(GB *gb) {
  pirate_captain_state0_tail_hook(gb);
}

void pirate_captain_state0_hook(GB *gb) {
  BASE(interactionCodec3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+14, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+17);
  interactionCodec3__afterCall7354_hook(gb);
}

void interactionCodec3_hook(GB *gb) {
  BASE(interactionCodec3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); pirate_captain_state0_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+8);
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  CYC(b_+11, b_+14); TAIL(interactionAnimate);
}
