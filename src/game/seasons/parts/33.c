#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/33.s.
// PART_33

static uint16_t i33_jump_table(GB *gb) {
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

// PART_33
void s_partCode33_hook(GB *gb) {
  BASE(partCode33);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i33_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+64) goto state1;
    if (jt_ == b_+85) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = PART_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x50);
  CYC(b_+17, b_+19); B = 0x00;
  CYC(b_+19, b_+22); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+22, b_+24); alu_and(gb, 0x30);
  if (F & FZ) { CYCT(b_+24, b_+26); goto L_691a; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); B = 0x20;
  CYC(b_+28, b_+30); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_691a; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); B = 0xe0;
L_691a:
  CYC(b_+34, b_+37); A = mem_rd(gb, w1Link_xh);
  CYC(b_+37, b_+38); alu_add(gb, B);
  CYC(b_+38, b_+39); C = A;
  CYC(b_+39, b_+41); alu_sub(gb, 0x08);
  CYC(b_+41, b_+43); alu_cp(gb, 0x90);
  if (F & FC) { CYCT(b_+43, b_+45); goto L_692d; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); C = 0x08;
  CYC(b_+47, b_+49); alu_cp(gb, 0xd0);
  if (!(F & FC)) { CYCT(b_+49, b_+51); goto L_692d; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); C = 0x98;
L_692d:
  CYC(b_+53, b_+55); B = 0xa0;
  CALL_C(b_+55, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+58);
  CYC(b_+58, b_+60); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+64);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+64, s_objectApplySpeed, SYM(objectApplySpeed), b_+67);
  CYC(b_+67, b_+69); E = PART_BASE + OBJ_YH;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+72); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(b_+72, b_+74); goto animate; }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = PART_BASE + OBJ_STATE;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+80); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x78);
animate:
  CYC(b_+82, b_+85);
  TAIL(partAnimate);
state2:
  CALL_C(b_+85, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+88);
  if (F & FZ) { CYCT(b_+88, b_+91); TAIL(partDelete); }
  CYC(b_+88, b_+91);
  CYC(b_+91, b_+93);
  goto animate;
}

