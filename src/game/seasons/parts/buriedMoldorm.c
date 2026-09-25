#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/buriedMoldorm.s.
// PART_BURIED_MOLDORM

// PART_BURIED_MOLDORM
void s_partCode2b_hook(GB *gb) {
  BASE(partCode2b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x9a);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(wTmpcfc0);
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+13, b_+16);
  TAIL(partDelete);
normalStatus:
  CYC(b_+16, b_+18); E = PART_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x12);
  CYC(b_+28, b_+30); L = PART_BASE + OBJ_VAR3F;
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  RET(b_+32); return;
}

