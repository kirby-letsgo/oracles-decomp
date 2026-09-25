#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/49.s.
// PART_49

static uint16_t i49_jump_table(GB *gb) {
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

// PART_49
void s_partCode49_hook(GB *gb) {
  BASE(partCode49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i49_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+25) goto state1;
    if (jt_ == b_+43) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); L = PART_BASE + OBJ_SPEED;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x6e);
  CYC(b_+18, b_+20); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x3c);
  CYC(b_+22, b_+25);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+25, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto animate; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); A = 0xd3;
  CALL_C(b_+34, s_playSound, SYM(playSound_b00), b_+37);
  CALL_C(b_+37, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+40);
  CYC(b_+40, b_+42); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
state2:
  CALL_C(b_+43, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+46);
  if (F & FZ) { CYCT(b_+46, b_+49); TAIL(partDelete); }
  CYC(b_+46, b_+49);
  CALL_C(b_+49, s_objectApplySpeed, SYM(objectApplySpeed), b_+52);
animate:
  CYC(b_+52, b_+55);
  TAIL(partAnimate);
}

