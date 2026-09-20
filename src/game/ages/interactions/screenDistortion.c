#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// INTERAC_SCREEN_DISTORTION
void interactionCode7c_hook(GB *gb) {
  BASE(interactionCode7c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // interactionCode7c@state1
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); loadBigBufferScrollValues_hook(gb); return; // jp

state0:
  CALL_C(b_+10, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+13);
  CALL_C(b_+13, interactionIncState_hook, SYM(interactionIncState), b_+16);
  CYC(b_+16, b_+18); A = 0x10;
  CYC(b_+18, b_+21); W8(wGfxRegs2_LYC) = A;
  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+25); H8(hNextLcdInterruptBehaviour) = A;
  CYC(b_+25, b_+27); A = 0x95; // SND_WARP_START
  CALL_C(b_+27, playSound_b00_hook, SYM(playSound_b00), b_+30);
  CYC(b_+30, b_+32); A = 0xff;
  CYC(b_+32, b_+35); initWaveScrollValues_hook(gb); return; // jp
}
