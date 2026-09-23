#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/4a.s.
// PART_4a

static uint16_t part4a_jump_table(GB *gb) {
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

// PART_4a
void s_partCode4a_hook(GB *gb) {
  BASE(partCode4a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); TAIL(partDelete); }
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (part4a_jump_table(gb));
    if (jt_ == b_+13) goto state0;
    if (jt_ == b_+29) goto state1;
    if (jt_ == b_+47) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); L = PART_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x5a);
  CYC(b_+20, b_+22); L = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+22, b_+24); A = 0x04;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+29, s_seasonsFunc_10_79ab_hook, SYM(seasonsFunc_10_79ab), b_+32);
  CYC(b_+32, b_+34); E = PART_BASE + OBJ_YH;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); alu_cp(gb, 0x88);
  if (F & FC) { CYCT(b_+37, b_+39); goto animate; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); E = PART_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); A = 0x02;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
animate:
  CYC(b_+44, b_+47);
  TAIL(partAnimate);
state2:
  CALL_C(b_+47, s_objectApplySpeed, SYM(objectApplySpeed), b_+50);
  CYC(b_+50, b_+52); E = PART_BASE + OBJ_YH;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+55); alu_cp(gb, 0xb8);
  if (F & FC) { CYCT(b_+55, b_+57); goto animate; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+60);
  TAIL(partDelete);
}

void s_seasonsFunc_10_79ab_hook(GB *gb) {
  BASE(seasonsFunc_10_79ab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_VAR31;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); B = 0x9a;
  CALL_C(b_+6, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+9);
  CYC(b_+9, b_+11); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15);
  TAIL(objectApplySpeed);
}

