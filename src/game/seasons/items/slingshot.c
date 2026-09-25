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


// ITEM_SLINGSHOT
void s_itemCode13_hook(GB *gb) {
  BASE(itemCode13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x1d;
  CALL_C(b_+7, s_loadWeaponGfx, SYM(loadWeaponGfx_b00), b_+10);
  CALL_C(b_+10, s_loadAttributesAndGraphicsAndIncState, SYM(loadAttributesAndGraphicsAndIncState), b_+13);
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+17); A = mem_rd(gb, wSlingshotLevel);
  CYC(b_+17, b_+19); alu_or(gb, 0x08);
  CYC(b_+19, b_+21); L = 0x1b;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+26);
  TAIL(objectSetVisible81);
}

