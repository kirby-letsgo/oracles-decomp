#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/3b.s.
// PART_3b

static void i3b_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// PART_3b
void s_partCode3b_hook(GB *gb) {
  BASE(partCode3b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_6c11; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
L_6c11:
  CYC(b_+8, b_+10); A = 0x01;
  CALL_C(b_+10, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x7e);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); TAIL(partDelete); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_and(gb, 0x80);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+27); E = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_and(gb, 0x7f);
  CYC(b_+30, b_+31); alu_or(gb, B);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+34, b_+36); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+36, b_+38); goto L_6c32; }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); A = A & 0x7f;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
L_6c32:
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+43, b_+44); B = mem_rd(gb, HL);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+46, b_+47); C = mem_rd(gb, HL);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+52); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+52, b_+54); goto L_6c43; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); alu_sub(gb, 0x04);
  CYC(b_+56, b_+57); alu_add(gb, A);
  CYC(b_+57, b_+58); A = alu_inc8(gb, A);
L_6c43:
  CYC(b_+58, b_+59); alu_add(gb, A);
  CYC(b_+59, b_+62); SET_HL(SYM(seasonsTable_10_6c5b));
  CYC(b_+62, b_+63); i3b_add_double_index(gb, b_+63);
  CYC(b_+63, b_+65); E = PART_BASE + OBJ_YH;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+66, b_+67); alu_add(gb, B);
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+70); E = PART_BASE + OBJ_XH;
  CYC(b_+70, b_+71); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+71, b_+72); alu_add(gb, C);
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+75); E = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+78); E = alu_inc8(gb, E);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+81); alu_xor(gb, A);
  RET(b_+81); return;
}
