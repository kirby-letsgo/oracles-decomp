#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/seasonSpiritsScripts.s.
// INTERAC_SEASON_SPIRITS_SCRIPTS

static uint16_t seasonSpiritsScripts_jump_table(GB *gb) {
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

static void seasonSpiritsScripts_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SEASON_SPIRITS_SCRIPTS
void s_interactionCode23_hook(GB *gb) {
  BASE(interactionCode23);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (seasonSpiritsScripts_jump_table(gb));
    if (jt_ == b_+8) goto substate0;
    if (jt_ == b_+57) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x08;
  CALL_C(b_+13, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+24); alu_and(gb, 0x0f);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); SET_HL(SYM(table_56a5));
  CYC(b_+30, b_+31); seasonSpiritsScripts_add_double_index(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+33); H = mem_rd(gb, HL);
  CYC(b_+33, b_+34); L = A;
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+37); alu_and(gb, 0x0f);
  CYC(b_+37, b_+38); seasonSpiritsScripts_add_double_index(gb, b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+39, b_+40); H = mem_rd(gb, HL);
  CYC(b_+40, b_+41); L = A;
  CALL_C(b_+41, s_interactionSetScript, SYM(interactionSetScript), b_+44);
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+46, b_+49); A = mem_rd(gb, wc6e5);
  CYC(b_+49, b_+51); alu_cp(gb, 0x09);
  CYC(b_+51, b_+53); A = 0x00;
  if (F & FC) { CYCT(b_+53, b_+55); goto L_569d; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); A = alu_inc8(gb, A);
L_569d:
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
substate1:
  CALL_C(b_+57, s_interactionRunScript, SYM(interactionRunScript), b_+60);
  if (F & FC) { CYCT(b_+60, b_+63); TAIL(interactionDelete); }
  CYC(b_+60, b_+63);
  RET(b_+63); return;
}
