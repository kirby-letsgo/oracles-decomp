#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/dodongoFireball.s.
// PART_DODONGO_FIREBALL

static void dodongoFireball_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// PART_DODONGO_FIREBALL
void s_partCode41_hook(GB *gb) {
  BASE(partCode41);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, s_objectApplySpeed, SYM(objectApplySpeed), b_+9);
  CALL_C(b_+9, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+15); TAIL(partDelete); }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+18);
  TAIL(partAnimate);
state0:
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x78);
  CYC(b_+26, b_+28); L = PART_BASE + OBJ_YH;
  CYC(b_+28, b_+30); A = 0x04;
  CYC(b_+30, b_+31); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_bit(gb, 3, A);
  CYC(b_+37, b_+39); E = PART_BASE + OBJ_YH;
  if (F & FZ) { CYCT(b_+39, b_+41); goto L_70d5; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); E = PART_BASE + OBJ_XH;
L_70d5:
  CYC(b_+43, b_+45); A = alu_swap(gb, A);
  CYC(b_+45, b_+46); alu_rlca(gb);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+50); SET_HL(SYM(table_70f3));
  CYC(b_+50, b_+51); dodongoFireball_add_a_to_hl(gb, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); A = B;
  CALL_C(b_+55, s_partSetAnimation, SYM(partSetAnimation), b_+58);
  CYC(b_+58, b_+60); A = 0x72;
  CALL_C(b_+60, s_playSound, SYM(playSound_b00), b_+63);
  CYC(b_+63, b_+65); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+67, b_+70); TAIL(objectSetVisible82); }
  CYC(b_+67, b_+70);
  CYC(b_+70, b_+73);
  TAIL(objectSetVisible81);
}
