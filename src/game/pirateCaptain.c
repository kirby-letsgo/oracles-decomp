#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void pirate_captain_state0_tail_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7354, objectSetVisible82_hook, 0x1e69, 0x7357);
  CALL_C(0x7357, checkIsLinkedGame_hook, 0x1992, 0x735a);
  if (!(F & FZ)) {
    CYCT(0x735a, 0x735c);
  } else {
    CYC(0x735a, 0x735c);
    CYC(0x735c, 0x735f); SET_HL(wTilesetFlags);
    CYC(0x735f, 0x7361); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  }
  CYC(0x7361, 0x7364); SET_HL(0x7dc1);
  CALL_C(0x7364, interactionSetScript_hook, 0x2544, 0x7367);
  CYC(0x7367, 0x736a); interactionIncState_hook(gb);
}

void interactionCodec3__afterCall7354_hook(GB *gb) {
  pirate_captain_state0_tail_hook(gb);
}

void pirate_captain_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7351, interactionInitGraphics_hook, 0x15fb, 0x7354);
  interactionCodec3__afterCall7354_hook(gb);
}

void interactionCodec3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7343, checkInteractionState_hook, 0x23fe, 0x7346);
  if (F & FZ) {
    CYCT(0x7346, 0x7348); pirate_captain_state0_hook(gb);
    return;
  }
  CYC(0x7346, 0x7348);
  CALL_C(0x7348, objectPreventLinkFromPassing_hook, 0x2680, 0x734b);
  CALL_C(0x734b, interactionRunScript_hook, 0x2552, 0x734e);
  CYC(0x734e, 0x7351); interactionAnimate_hook(gb);
}
