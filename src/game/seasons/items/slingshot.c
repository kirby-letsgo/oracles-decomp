#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/items/slingshot.s.
// In common folder because Ages has a stub

void s_foolsOreRet_hook(GB *gb) {
  BASE(foolsOreRet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

