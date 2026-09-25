#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/itemParents/commonCode.s.

// checkNoOtherParentItemsInUse@nextItem
static void commonCode_nextItem(GB *gb) {
  BASE(checkNoOtherParentItemsInUse);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto nextItem;
L_52d3:
  CYC(b_+3, b_+4); A = D;
  CYC(b_+4, b_+5); alu_cp(gb, H);
  if (F & FZ) { CYCT(b_+5, b_+7); goto nextItem; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
nextItem:
  CYC(b_+10, b_+11); H = alu_inc8(gb, H);
  CYC(b_+11, b_+12); A = H;
  CYC(b_+12, b_+14); alu_cp(gb, 0xd6);
  if (F & FC) { CYCT(b_+14, b_+16); goto L_52d3; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  RET(b_+17); return;
}

// Unused function (in both ages and seasons)
void s_func_5358_hook(GB *gb) {
  BASE(func_5358);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkNoOtherParentItemsInUse, SYM(checkNoOtherParentItemsInUse), b_+3);
L_52c5:
  CYC(b_+3, b_+4); push_effect(gb, HL);
  if (!(F & FZ)) CALL_C_CC(b_+4, s_clearParentItemH, SYM(clearParentItemH), b_+7);
  else CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  CALL_L(b_+8, commonCode_nextItem, b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_52c5; }
  CYC(b_+11, b_+13);
  RET(b_+13); return;
}
