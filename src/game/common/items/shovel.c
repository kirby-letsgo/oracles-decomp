#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemCode15_hook(GB *gb) {
  BASE(itemCode15);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto state1;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+9);
  CALL_C(b_+9, itemIncState_hook, SYM(itemIncState), b_+12);
  CYC(b_+12, b_+14); L = 0x06;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x04);
  CYC(b_+16, b_+18); A = 0x06;
  CALL_C(b_+18, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+21);
  CYC(b_+21, b_+23); A = 0x50;
  if (!(F & FC)) {
    CYCT(b_+23, b_+25);
  } else {
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+27); A = 0x01;
    CALL_C(b_+27, addToGashaMaturity_hook, SYM(addToGashaMaturity), b_+30);
    CYC(b_+30, b_+32); A = 0xa9;
  }
  CYC(b_+32, b_+35); playSound_b00_hook(gb); return;

state1:
  CALL_C(b_+35, itemDecCounter1_hook, SYM(itemDecCounter1), b_+38);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+39); ret_effect(gb); return;
  }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+42); itemDelete_hook(gb);
}
