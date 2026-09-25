#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/poeSisterFlame.s.
// PART_POE_SISTER_FLAME

// PART_POE_SISTER_FLAME
void s_partCode3c_hook(GB *gb) {
  BASE(partCode3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_BC(0x0104);
  CALL_C(b_+9, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+12);
  if (F & FZ) { CYCT(b_+12, b_+14); goto delete; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_cp(gb, 0x46);
  if (F & FZ) { CYCT(b_+17, b_+19); goto L_6c86; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); SET_BC(0x0206);
  CYC(b_+22, b_+24); alu_cp(gb, 0x28);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); TAIL(partAnimate); }
  CYC(b_+24, b_+27);
L_6c86:
  CYC(b_+27, b_+29); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); mem_wr(gb, HL, C);
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+36);
  TAIL(partSetAnimation);
delete:
  CYC(b_+36, b_+37); SET_HL(pop_effect(gb));
  CYC(b_+37, b_+40);
  TAIL(partDelete);
state0:
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+42); L = E;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+45); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x64);
  CYC(b_+47, b_+50);
  TAIL(objectSetVisible83);
}

