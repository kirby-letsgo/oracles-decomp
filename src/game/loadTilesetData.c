#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

// 04:6dd6
void getAdjustedRoomGroup_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;

  CYC(0x6dd6, 0x6dd9); A = mem_rd(gb, wActiveGroup);
  CYC(0x6dd9, 0x6dda); B = A;
  CYC(0x6dda, 0x6ddc); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(0x6ddc, 0x6ddd); ret_effect(gb);
    return;
  }
  CYC(0x6ddc, 0x6ddd);

  CALL_C(0x6ddd, getThisRoomFlags_hook, 0x197d, 0x6de0);
  CYC(0x6de0, 0x6de1); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6de1, 0x6de3);
  } else {
    CYC(0x6de1, 0x6de3);
    CYC(0x6de3, 0x6de5); B |= 0x02;
  }
  CYC(0x6de5, 0x6de6); A = B;
  CYC(0x6de6, 0x6de7); ret_effect(gb);
}
