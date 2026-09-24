#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/smallVolcano.s.
// INTERAC_SMALL_VOLCANO

static void smallVolcano_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SMALL_VOLCANO
void s_interactionCode51_hook(GB *gb) {
  BASE(interactionCode51);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wFrameCounter);
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+12); A = 0xb3;
  if (F & FZ) CALL_C_CC(b_+12, s_playSound, SYM(playSound_b00), b_+15);
  else CYC(b_+12, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_7a59; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wScreenShakeCounterX);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+25, s_func_7a9a_hook, SYM(func_7a9a), b_+28);
  else CYC(b_+25, b_+28);
L_7a59:
  CALL_C(b_+28, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+31);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CALL_C(b_+32, s_func_7abe_hook, SYM(func_7abe), b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, A);
  CYC(b_+39, b_+41); C = 0x07;
  if (F & FZ) { CYCT(b_+41, b_+43); goto L_7a6a; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); C = 0x0f;
L_7a6a:
  CALL_C(b_+45, s_getRandomNumber, SYM(getRandomNumber), b_+48);
  CYC(b_+48, b_+49); alu_and(gb, C);
  CYC(b_+49, b_+51); C = alu_srl(gb, C);
  CYC(b_+51, b_+52); C = alu_inc8(gb, C);
  CYC(b_+52, b_+53); alu_sub(gb, C);
  CYC(b_+53, b_+54); C = A;
  CALL_C(b_+54, s_getFreePartSlot, SYM(getFreePartSlot), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x11);
  CYC(b_+60, b_+62); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+67); B = 0x00;
  CYC(b_+67, b_+70);
  TAIL(objectCopyPositionWithOffset);
state0:
  CYC(b_+70, b_+71); A = alu_inc8(gb, A);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+75); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(b_+75, b_+77); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+77, b_+78); A = mem_rd(gb, DE);
  CYC(b_+78, b_+81); SET_HL(SYM(table_7acb));
  CYC(b_+81, b_+82); smallVolcano_add_double_index(gb, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+83, b_+84); H = mem_rd(gb, HL);
  CYC(b_+84, b_+85); L = A;
  CYC(b_+85, b_+87); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+87, b_+88); A = L;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+90); E = alu_inc8(gb, E);
  CYC(b_+90, b_+91); A = H;
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  RET(b_+92); return;
}

void s_func_7a9a_hook(GB *gb) {
  BASE(func_7a9a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+9); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_7aa9; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+15);
  TAIL(interactionDelete);
L_7aa9:
  CYC(b_+15, b_+18); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+22); mem_wr(gb, wScreenShakeCounterX, A);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+31, b_+32); A = L;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); E = alu_inc8(gb, E);
  CYC(b_+34, b_+35); A = H;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  s_func_7abe_hook(gb); return; // falls through
}

void s_func_7abe_hook(GB *gb) {
  BASE(func_7abe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+6, b_+7); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  RET(b_+12); return;
}

