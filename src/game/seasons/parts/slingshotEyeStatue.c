#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/slingshotEyeStatue.s.
// PART_SLINGSHOT_EYE_STATUE

static uint16_t slingshotEyeStatue_jump_table(GB *gb) {
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

// PART_SLINGSHOT_EYE_STATUE
void s_partCode0d_hook(GB *gb) {
  BASE(partCode0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CALL_C(b_+2, s_objectSetVisible83, SYM(objectSetVisible83), b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x2d);
  CYC(b_+10, b_+12); L = PART_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_and(gb, 0x07);
  CYC(b_+16, b_+19); SET_HL(wActiveTriggers);
  CALL_C(b_+19, s_setFlag, SYM(setFlag), b_+22);
  CYC(b_+22, b_+24); alu_bit(gb, 7, B);
  if (F & FZ) { CYCT(b_+24, b_+26); goto normalStatus; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); E = PART_BASE + OBJ_STATE;
  CYC(b_+28, b_+30); A = 0x02;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
normalStatus:
  CYC(b_+31, b_+33); E = PART_BASE + OBJ_STATE;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (slingshotEyeStatue_jump_table(gb));
    if (jt_ == b_+41) goto state0;
    if (jt_ == b_+45) goto state1;
    if (jt_ == SYM(objectSetVisible83) && hook_is(gb, SYM(objectSetVisible83), s_objectSetVisible83)) { s_objectSetVisible83(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  RET(b_+44); return;
state1:
  CALL_C(b_+45, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); E = PART_BASE + OBJ_SUBID;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+55); SET_HL(wActiveTriggers);
  CALL_C(b_+55, s_unsetFlag, SYM(unsetFlag), b_+58);
  CYC(b_+58, b_+61);
  TAIL(objectSetInvisible);
}

