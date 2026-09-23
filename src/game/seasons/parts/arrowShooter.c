#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/arrowShooter.s.
// PART_WALL_ARROW_SHOOTER

static void arrowShooter_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// PART_WALL_ARROW_SHOOTER
void s_partCode25_hook(GB *gb) {
  BASE(partCode25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_65da; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = PART_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
L_65da:
  CALL_C(b_+18, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = PART_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_bit(gb, 0, A);
  CYC(b_+27, b_+29); E = PART_BASE + OBJ_XH;
  CYC(b_+29, b_+31); A = mem_rd(gb, hEnemyTargetX);
  if (F & FZ) { CYCT(b_+31, b_+33); goto L_65ed; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); E = PART_BASE + OBJ_YH;
  CYC(b_+35, b_+37); A = mem_rd(gb, hEnemyTargetY);
L_65ed:
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_sub(gb, B);
  CYC(b_+40, b_+42); alu_add(gb, 0x10);
  CYC(b_+42, b_+44); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); A = 0x21;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); SET_HL(SYM(table_6661));
  CYC(b_+53, b_+55);
  TAIL_S(func_6649);
}

// PART_CANNON_ARROW_SHOOTER
void s_partCode2c_hook(GB *gb) {
  BASE(partCode2c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); TAIL_S(label_10_270); }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = PART_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+18); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); A = B;
  CALL_C(b_+20, s_partSetAnimation, SYM(partSetAnimation), b_+23);
  CALL_C(b_+23, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x30);
  CYC(b_+28, b_+30); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, s_objectMakeTileSolid, SYM(objectMakeTileSolid), b_+34);
  CYC(b_+34, b_+37);
  TAIL(objectSetVisible82);
}

void s_label_10_270_hook(GB *gb) {
  BASE(label_10_270);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hCameraY);
  CYC(b_+2, b_+4); alu_add(gb, 0x80);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); E = PART_BASE + OBJ_YH;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_cp(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = mem_rd(gb, hCameraX);
  CYC(b_+12, b_+14); alu_add(gb, 0xa0);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+17); E = PART_BASE + OBJ_XH;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_cp(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CALL_C(b_+20, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CALL_C(b_+24, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+27);
  CYC(b_+27, b_+29); alu_and(gb, 0x60);
  CYC(b_+29, b_+31); alu_add(gb, 0x20);
  CYC(b_+31, b_+33); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+37); SET_HL(SYM(table_6669));
  s_func_6649_hook(gb); return; // falls through
}

void s_func_6649_hook(GB *gb) {
  BASE(func_6649);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); arrowShooter_add_double_index(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CALL_C(b_+7, s_getFreePartSlot, SYM(getFreePartSlot), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1a);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+15, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+18);
  CYC(b_+18, b_+20); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); E = L;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

