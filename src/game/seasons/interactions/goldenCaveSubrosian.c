#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/goldenCaveSubrosian.s.
// INTERAC_GOLDEN_CAVE_SUBROSIAN

static uint16_t goldenCaveSubrosian_jump_table(GB *gb) {
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

static void goldenCaveSubrosian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GOLDEN_CAVE_SUBROSIAN
void s_interactionCodecc_hook(GB *gb) {
  BASE(interactionCodecc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (goldenCaveSubrosian_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+80) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x28;
  CALL_C(b_+10, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+13);
  if (F & FZ) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); A = 0x4c;
  CALL_C(b_+21, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+24);
  CALL_C(b_+24, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+27);
  CYC(b_+27, b_+29); alu_and(gb, 0x03);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_7d46; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); SET_HL(SYM(seasonsTable_0f_7dc7));
  CYC(b_+35, b_+36); goldenCaveSubrosian_add_double_index(gb, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+38); H = mem_rd(gb, HL);
  CYC(b_+38, b_+39); L = A;
  CYC(b_+39, b_+41);
  goto setScript;
L_7d46:
  CYC(b_+41, b_+43); A = 0x5c;
  CALL_C(b_+43, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+46);
  if (F & FZ) { CYCT(b_+46, b_+48); goto notDoneSubrosianScript; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); SET_HL(SYM(seasonsFunc_0f_7dac));
  CYC(b_+51, b_+53);
  goto setScript;
notDoneSubrosianScript:
  CALL_C(b_+53, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+56);
  CYC(b_+56, b_+58); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+58, b_+60); goto notGivenSecret; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+63); SET_HL(SYM(goldenCaveSubrosianScript_givenSecret));
  CYC(b_+63, b_+65);
  goto setScript;
notGivenSecret:
  CYC(b_+65, b_+68); SET_HL(SYM(goldenCaveSubrosianScript_beginningSecret));
setScript:
  CALL_C(b_+68, s_interactionSetScript, SYM(interactionSetScript), b_+71);
  CALL_C(b_+71, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+74);
  CALL_C(b_+74, s_seasonsFunc_0f_7dc1_hook, SYM(seasonsFunc_0f_7dc1), b_+77);
  CALL_C(b_+77, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+80);
state1:
  CALL_C(b_+80, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+83);
  CALL_C(b_+83, s_interactionRunScript, SYM(interactionRunScript), b_+86);
  CALL_C(b_+86, s_seasonsFunc_0f_7dac_hook, SYM(seasonsFunc_0f_7dac), b_+89);
  CALL_C(b_+89, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+92);
  if (!(F & FZ)) { RET_TAKEN(b_+92); return; }
  CYC(b_+92, b_+93);
  CALL_C(b_+93, s_func_7d95_hook, SYM(func_7d95), b_+96);
  CYC(b_+96, b_+98); A = 0xf8;
  CALL_C(b_+98, s_findTileInRoom, SYM(findTileInRoom), b_+101);
  if (F & FZ) { RET_TAKEN(b_+101); return; }
  CYC(b_+101, b_+102);
  CALL_C(b_+102, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+105);
  CYC(b_+105, b_+107); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+107, b_+108); A = mem_rd(gb, HL);
  CYC(b_+108, b_+110); B = 0x02;
  CYC(b_+110, b_+112); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+112, b_+114); goto L_7d91; }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); B = 0x03;
L_7d91:
  CYC(b_+116, b_+118); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+118, b_+119); mem_wr(gb, HL, B);
  RET(b_+119); return;
}

void s_func_7d95_hook(GB *gb) {
  BASE(func_7d95);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x06;
  CALL_C(b_+2, s_findItemWithID, SYM(findItemWithID), b_+5);
  CYC(b_+5, b_+6); H = D;
  if (F & FZ) { CYCT(b_+6, b_+8); goto failed; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  RET(b_+12); return;
failed:
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x01);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+22); return;
}

void s_seasonsFunc_0f_7dac_hook(GB *gb) {
  BASE(seasonsFunc_0f_7dac);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); alu_or(gb, A);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); C = 0x20;
  CALL_C(b_+17, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  s_seasonsFunc_0f_7dc1_hook(gb); return; // falls through
}

void s_seasonsFunc_0f_7dc1_hook(GB *gb) {
  BASE(seasonsFunc_0f_7dc1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(IO_LCDC);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

