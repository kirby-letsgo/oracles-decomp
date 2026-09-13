#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemCode15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c1f, 0x5c21); E = 0x04;
  CYC(0x5c21, 0x5c22); A = mem_rd(gb, DE);
  CYC(0x5c22, 0x5c23); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5c23, 0x5c25);
    goto state1;
  }
  CYC(0x5c23, 0x5c25);
  CALL_C(0x5c25, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5c28);
  CALL_C(0x5c28, itemIncState_hook, 0x23ea, 0x5c2b);
  CYC(0x5c2b, 0x5c2d); L = 0x06;
  CYC(0x5c2d, 0x5c2f); mem_wr(gb, HL, 0x04);
  CYC(0x5c2f, 0x5c31); A = 0x06;
  CALL_C(0x5c31, itemTryToBreakTile_hook, 0x2bef, 0x5c34);
  CYC(0x5c34, 0x5c36); A = 0x50;
  if (!(F & FC)) {
    CYCT(0x5c36, 0x5c38);
  } else {
    CYC(0x5c36, 0x5c38);
    CYC(0x5c38, 0x5c3a); A = 0x01;
    CALL_C(0x5c3a, addToGashaMaturity_hook, 0x1821, 0x5c3d);
    CYC(0x5c3d, 0x5c3f); A = 0xa9;
  }
  CYC(0x5c3f, 0x5c42); playSound_b00_hook(gb); return;

state1:
  CALL_C(0x5c42, itemDecCounter1_hook, 0x23d6, 0x5c45);
  if (!(F & FZ)) {
    CYCT(0x5c45, 0x5c46); ret_effect(gb); return;
  }
  CYC(0x5c45, 0x5c46);
  CYC(0x5c46, 0x5c49); itemDelete_hook(gb);
}
