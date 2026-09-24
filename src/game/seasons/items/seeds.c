#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/items/seeds.s.

// @param[out]	zflag	z if no collision
void s_slingshotCheckCanPassSolidTile_hook(GB *gb) {
  BASE(slingshotCheckCanPassSolidTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectCheckTileCollision_allowHoles, SYM(objectCheckTileCollision_allowHoles), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); goto L_4fe8; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_itemCheckCanPassSolidTile, SYM(itemCheckCanPassSolidTile), b_+8);
  RET(b_+8); return;
L_4fe8:
  CYC(b_+9, b_+10); alu_xor(gb, A);
  RET(b_+10); return;
}

