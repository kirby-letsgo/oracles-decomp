#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/2f.s.
// PART_2f

// PART_2f
void s_partCode2f_hook(GB *gb) {
  BASE(partCode2f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = PART_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_sub(gb, 0x02);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = PART_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_add(gb, 0x03);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  RET(b_+19); return;
}

