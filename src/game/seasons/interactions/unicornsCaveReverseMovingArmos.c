#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/unicornsCaveReverseMovingArmos.s.
// INTERAC_D5_REVERSE_MOVING_ARMOS

static uint16_t unicornsCaveReverseMovingArmos_jump_table(GB *gb) {
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

static void unicornsCaveReverseMovingArmos_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_D5_REVERSE_MOVING_ARMOS
void s_interactionCode63_hook(GB *gb) {
  BASE(interactionCode63);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (unicornsCaveReverseMovingArmos_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+93) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+11); A = mem_rd(gb, wBlockPushAngle);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); alu_add(gb, 0x10);
  CYC(b_+15, b_+17); alu_and(gb, 0x1f);
  CYC(b_+17, b_+19); alu_add(gb, 0x04);
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+24); alu_and(gb, 0x03);
  CYC(b_+24, b_+27); SET_HL(b_+89 /* @table_50da */);
  CYC(b_+27, b_+28); unicornsCaveReverseMovingArmos_add_a_to_hl(gb, b_+28);
  CYC(b_+28, b_+29); C = mem_rd(gb, HL);
  CALL_C(b_+29, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+32);
  CYC(b_+32, b_+33); alu_add(gb, C);
  CYC(b_+33, b_+35); B = 0xce;
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+38); mem_wr(gb, hFF8C, A);
  CYC(b_+38, b_+39); A = mem_rd(gb, BC);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto func_50e3; }
  CYC(b_+40, b_+42);
  CALL_C(b_+42, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+45);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x14);
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+50, b_+53); A = mem_rd(gb, wBlockPushAngle);
  CYC(b_+53, b_+55); alu_add(gb, 0x10);
  CYC(b_+55, b_+57); alu_and(gb, 0x1f);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); mem_wr(gb, hFF8B, A);
  CYC(b_+60, b_+63); SET_BC(0xfe00);
  CALL_C(b_+63, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+66);
  CALL_C(b_+66, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+69);
  CYC(b_+69, b_+71); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+75, b_+77); A = mem_rd(gb, hFF8C);
  CALL_C(b_+77, s_setShortPosition, SYM(setShortPosition), b_+80);
  CYC(b_+80, b_+82); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x01);
  CYC(b_+84, b_+85); alu_xor(gb, A);
  CYC(b_+85, b_+88); mem_wr(gb, wBlockPushAngle, A);
  RET(b_+88); return;

state1:
  CYC(b_+93, b_+96); A = mem_rd(gb, wBlockPushAngle);
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+97); return; }
  CYC(b_+97, b_+98);
func_50e3:
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+100, b_+101); alu_xor(gb, A);
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+105); mem_wr(gb, wBlockPushAngle, A);
  RET(b_+105); return;
}

