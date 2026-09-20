#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// INTERAC_SCREEN_DISTORTION
void interactionCode7c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6167, checkInteractionState_hook, 0x23fe, 0x616a);
  if (F & FZ) { CYCT(0x616a, 0x616c); goto state0; } // jr z
  CYC(0x616a, 0x616c);

  // interactionCode7c@state1
  CYC(0x616c, 0x616e); A = 0x01;
  CYC(0x616e, 0x6171); loadBigBufferScrollValues_hook(gb); return; // jp

state0:
  CALL_C(0x6171, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6174);
  CALL_C(0x6174, interactionIncState_hook, 0x23e0, 0x6177);
  CYC(0x6177, 0x6179); A = 0x10;
  CYC(0x6179, 0x617c); W8(wGfxRegs2_LYC) = A;
  CYC(0x617c, 0x617e); A = 0x02;
  CYC(0x617e, 0x6180); H8(hNextLcdInterruptBehaviour) = A;
  CYC(0x6180, 0x6182); A = 0x95; // SND_WARP_START
  CALL_C(0x6182, playSound_b00_hook, 0x0c98, 0x6185);
  CYC(0x6185, 0x6187); A = 0xff;
  CYC(0x6187, 0x618a); initWaveScrollValues_hook(gb); return; // jp
}
