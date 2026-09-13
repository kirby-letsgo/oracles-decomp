#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemCode1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e5a, 0x5e5c); E = 0x04;
  CYC(0x5e5c, 0x5e5d); A = mem_rd(gb, DE);
  CYC(0x5e5d, 0x5e5e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5e5e, 0x5e5f); ret_effect(gb); return; }
  CYC(0x5e5e, 0x5e5f);
  CALL_C(0x5e5f, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5e62);
  CALL_C(0x5e62, itemIncState_hook, 0x23ea, 0x5e65);
  CYC(0x5e65, 0x5e67); L = 0x00;
  CYC(0x5e67, 0x5e69); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x5e69, 0x5e6a); ret_effect(gb);
}

void itemCode1dPost_hook(GB *gb) {
  CYC(0x5e6a, 0x5e6d); SET_HL(w1Companion_id);
  CYC(0x5e6d, 0x5e6e); A = mem_rd(gb, HL);
  CYC(0x5e6e, 0x5e70); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x5e70, 0x5e73);
    objectTakePosition_hook(gb);
    return;
  }
  CYC(0x5e70, 0x5e73);
  CYC(0x5e73, 0x5e76);
  itemDelete_hook(gb);
}
