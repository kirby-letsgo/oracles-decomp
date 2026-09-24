#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank4.s.

void s_label_04_035_hook(GB *gb) {
  BASE(label_04_035);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_loadScreenMusicAndSetRoomPack, SYM(loadScreenMusicAndSetRoomPack), b_+3);
  CYC(b_+3, b_+6);
  TAIL_S(checkRoomPackAfterWarp);
}

void s_label_04_036_hook(GB *gb) {
  BASE(label_04_036);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(loadScreenMusicAndSetRoomPack);
}

