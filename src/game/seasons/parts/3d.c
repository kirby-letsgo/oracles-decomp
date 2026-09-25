#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/3d.s.
// PART_3d

static uint16_t i3d_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// partCode3d@func_6cf4
static void i3d_func_6cf4(GB *gb) {
  BASE(partCode3d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+87, b_+89); A = 0x6f;
  CALL_C(b_+89, s_playSound, SYM(playSound_b00), b_+92);
  CYC(b_+92, b_+94); A = 0x01;
  CYC(b_+94, b_+97);
  TAIL(partSetAnimation);
}

// PART_3d
void s_partCode3d_hook(GB *gb) {
  BASE(partCode3d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i3d_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+54) goto state1;
    if (jt_ == b_+97) goto state2;
    if (jt_ == b_+111) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); E = PART_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_6cbd; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+23, b_+25); A = 0x40;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xff);
  CYC(b_+28, b_+30); L = PART_BASE + OBJ_SPEED;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x3c);
L_6cbd:
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+35, b_+37); goto L_6cd0; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); L = PART_BASE + OBJ_STATE;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+42, b_+44); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+44, b_+46); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x1e);
  CALL_L(b_+48, i3d_func_6cf4, b_+51);
L_6cd0:
  CYC(b_+51, b_+54);
  TAIL(objectSetVisiblec1);
state1:
  CALL_C(b_+54, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+60); TAIL(partDelete); }
  CYC(b_+57, b_+60);
  CALL_C(b_+60, s_objectApplySpeed, SYM(objectApplySpeed), b_+63);
  CYC(b_+63, b_+65); C = 0x0e;
  CALL_C(b_+65, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto animate; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); L = PART_BASE + OBJ_STATE;
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+73, b_+75); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0xa0);
  CYC(b_+77, b_+79); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x05);
  CYC(b_+81, b_+82); L = alu_inc8(gb, L);
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x04);
  CALL_C(b_+84, s_func_6e13_hook, SYM(func_6e13), b_+87);
  CYC(b_+87, b_+89); A = 0x6f;
  CALL_C(b_+89, s_playSound, SYM(playSound_b00), b_+92);
  CYC(b_+92, b_+94); A = 0x01;
  CYC(b_+94, b_+97);
  TAIL(partSetAnimation);
state2:
  CALL_C(b_+97, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+100);
  if (!(F & FZ)) { CYCT(b_+100, b_+102); goto animate; }
  CYC(b_+100, b_+102);
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x14);
  CYC(b_+104, b_+105); L = E;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+108); A = 0x02;
  CYC(b_+108, b_+111);
  TAIL(partSetAnimation);
state3:
  CALL_C(b_+111, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+114);
  if (F & FZ) { CYCT(b_+114, b_+117); TAIL(partDelete); }
  CYC(b_+114, b_+117);
animate:
  CYC(b_+117, b_+120);
  TAIL(partAnimate);
}

