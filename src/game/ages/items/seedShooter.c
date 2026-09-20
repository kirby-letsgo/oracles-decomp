#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemInitializeFromLinkPosition_hook(GB *gb);

void itemCode0fPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b6a, cpRelatedObject1ID_hook, 0x4b86, 0x5b6d);
  if (!(F & FZ)) {
    CYCT(0x5b6d, 0x5b70);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x5b6d, 0x5b70);
  CYC(0x5b70, 0x5b73); SET_HL(0x5b88);
  CALL_C(0x5b73, itemInitializeFromLinkPosition_hook, 0x60d6, 0x5b76);
  CYC(0x5b76, 0x5b77); H = D;
  CYC(0x5b77, 0x5b7a); A = W8(w1Link_zh);
  CYC(0x5b7a, 0x5b7c); L = 0x0f;
  CYC(0x5b7c, 0x5b7d); mem_wr(gb, HL, A);
  CYC(0x5b7d, 0x5b7f); L = 0x30;
  CYC(0x5b7f, 0x5b82); A = W8(w1ParentItem2_angle);
  CYC(0x5b82, 0x5b83); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x5b83, 0x5b84); mem_wr(gb, HL, A);
  if (F & FZ) {
    CYCT(0x5b84, 0x5b85); ret_effect(gb); return;
  }
  CYC(0x5b84, 0x5b85);
  CYC(0x5b85, 0x5b88);
  itemSetAnimation_hook(gb);
}
