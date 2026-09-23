#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/poppableBubble.s.
// PART_POPPABLE_BUBBLE

static uint16_t poppableBubble_jump_table(GB *gb) {
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

static void poppableBubble_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// PART_POPPABLE_BUBBLE
void s_partCode32_hook(GB *gb) {
  BASE(partCode32);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); TAIL_S(func_68d5); }
  CYC(b_+6, b_+8);
  CALL_C(b_+8, s_func_68cc_hook, SYM(func_68cc), b_+11);
  CYC(b_+11, b_+13); A = 0xaf;
  CYC(b_+13, b_+16);
  TAIL(playSound_b00);
normalStatus:
  CYC(b_+16, b_+18); E = PART_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (poppableBubble_jump_table(gb));
    if (jt_ == b_+24) goto state0;
    if (jt_ == b_+66) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = PART_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CALL_C(b_+30, s_partSetAnimation, SYM(partSetAnimation), b_+33);
  CALL_C(b_+33, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+36);
  CYC(b_+36, b_+38); alu_and(gb, 0x03);
  CYC(b_+38, b_+41); SET_HL(b_+62 /* @table_68b9 */);
  CYC(b_+41, b_+42); poppableBubble_add_a_to_hl(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); E = PART_BASE + OBJ_SPEED;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CALL_C(b_+46, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x3f);
  CYC(b_+51, b_+53); alu_add(gb, 0x78);
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+56); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+57); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x10);
  CYC(b_+59, b_+62);
  TAIL(objectSetVisible81);

state1:
  CYC(b_+66, b_+68); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); TAIL_S(func_68d5); }
  CYC(b_+70, b_+72);
  CALL_C(b_+72, s_objectApplySpeed, SYM(objectApplySpeed), b_+75);
  CALL_C(b_+75, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+81); TAIL_S(seasonsFunc_10_68e0); }
  CYC(b_+78, b_+81);
  s_func_68cc_hook(gb); return; // falls through
}

void s_func_68cc_hook(GB *gb) {
  BASE(func_68cc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); A = 0x02;
  CYC(b_+6, b_+9);
  TAIL(partSetAnimation);
}

void s_func_68d5_hook(GB *gb) {
  BASE(func_68d5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_partAnimate, SYM(partAnimate), b_+3);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); TAIL(partDelete); }
  CYC(b_+7, b_+10);
  RET(b_+10); return;
}

void s_seasonsFunc_10_68e0_hook(GB *gb) {
  BASE(seasonsFunc_10_68e0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CALL_C(b_+7, s_getRandomNumber, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_68f3; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0xff;
L_68f3:
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

