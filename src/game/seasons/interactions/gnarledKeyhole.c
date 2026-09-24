#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/gnarledKeyhole.s.
// INTERAC_GNARLED_KEYHOLE

static uint16_t gnarledKeyhole_jump_table(GB *gb) {
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

static void gnarledKeyhole_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void gnarledKeyhole_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GNARLED_KEYHOLE
void s_interactionCode21_hook(GB *gb) {
  BASE(interactionCode21);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gnarledKeyhole_jump_table(gb));
    if (jt_ == b_+14) goto state0;
    if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), s_interactionRunScript)) { s_interactionRunScript(gb); return; }
    if (jt_ == b_+41) goto state2;
    if (jt_ == b_+62) goto state3;
    if (jt_ == b_+116) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+14, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL(interactionDelete); }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); A = 0x01;
  CYC(b_+29, b_+32); mem_wr(gb, wDisableWarpTiles, A);
  CALL_C(b_+32, s_func_5469_hook, SYM(func_5469), b_+35);
  CYC(b_+35, b_+38); SET_HL(SYM(gnarledKeyholeScript));
  CYC(b_+38, b_+41);
  TAIL(interactionSetScript);
state2:
  CALL_C(b_+41, s_interactionIncState, SYM(interactionIncState), b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+48, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+51);
  CYC(b_+51, b_+54); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+54, b_+56); alu_or(gb, 0x80);
  CYC(b_+56, b_+59); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+59, s_func_545d_hook, SYM(func_545d), b_+62);
state3:
  CALL_C(b_+62, s_func_54ae_hook, SYM(func_54ae), b_+65);
  CALL_C(b_+65, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); TAIL_S(func_545d); }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+75, b_+77); goto L_5428; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CALL_C(b_+79, s_func_549d_hook, SYM(func_549d), b_+82);
  CYC(b_+82, b_+84); A = 0x82;
  CALL_C(b_+84, s_playSound, SYM(playSound_b00), b_+87);
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);
  CYC(b_+90, b_+93); SET_HL(b_+106 /* @table_542d */);
  CYC(b_+93, b_+94); gnarledKeyhole_add_double_index(gb, b_+94);
  CYC(b_+94, b_+95); E = alu_dec8(gb, E);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+99); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+99, b_+101); TAIL_S(func_5463); }
  CYC(b_+99, b_+101);
L_5428:
  CYC(b_+101, b_+103); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+103, b_+104); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+104, b_+106);
  TAIL_S(func_5463);

state4:
  CYC(b_+116, b_+118); A = 0x09;
  CYC(b_+118, b_+121); SET_HL(SYM(table_5482));
  CYC(b_+121, b_+124); SET_BC(SYM(table_5494));
  CALL_C(b_+124, s_func_5471_hook, SYM(func_5471), b_+127);
  CYC(b_+127, b_+128); alu_xor(gb, A);
  CYC(b_+128, b_+131); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+131, b_+134); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+134, b_+137); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+137, b_+139); A = 0x4d;
  CALL_C(b_+139, s_playSound, SYM(playSound_b00), b_+142);
  CYC(b_+142, b_+145); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+145, b_+148); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+148, s_playSound, SYM(playSound_b00), b_+151);
  CYC(b_+151, b_+154);
  TAIL(interactionDelete);
}

void s_func_545d_hook(GB *gb) {
  BASE(func_545d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0f;
  CYC(b_+2, b_+5); mem_wr(gb, wScreenShakeCounterX, A);
  RET(b_+5); return;
}

void s_func_5463_hook(GB *gb) {
  BASE(func_5463);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+5); mem_wr(gb, wScreenShakeCounterY, A);
  RET(b_+5); return;
}

void s_func_5469_hook(GB *gb) {
  BASE(func_5469);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x09;
  CYC(b_+2, b_+5); SET_HL(SYM(table_5482));
  CYC(b_+5, b_+8); SET_BC(SYM(table_548b));
  s_func_5471_hook(gb); return; // falls through
}

void s_func_5471_hook(GB *gb) {
  BASE(func_5471);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); D = 0xce;
  CYC(b_+2, b_+3); E = A;
L_5474:
  CYC(b_+3, b_+4); push_effect(gb, DE);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); E = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); SET_BC(BC + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); SET_DE(pop_effect(gb));
  CYC(b_+10, b_+11); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_5474; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = mem_rd(gb, hActiveObject);
  CYC(b_+15, b_+16); D = A;
  RET(b_+16); return;
}

void s_func_549d_hook(GB *gb) {
  BASE(func_549d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(table_54a9));
  CYC(b_+3, b_+4); gnarledKeyhole_add_a_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CALL_C(b_+5, s_uniqueGfxFunc_380b, SYM(uniqueGfxFunc_380b), b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hActiveObject);
  CYC(b_+10, b_+11); D = A;
  RET(b_+11); return;
}

void s_func_54ae_hook(GB *gb) {
  BASE(func_54ae);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+9);
  CYC(b_+9, b_+10); E = A;
  CALL_C(b_+10, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x4b);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+20); A = mem_rd(gb, wFrameCounter);
  CYC(b_+20, b_+22); alu_and(gb, 0x06);
  CYC(b_+22, b_+23); alu_rrca(gb);
  CYC(b_+23, b_+26); SET_BC(SYM(table_54e4));
  CALL_C(b_+26, s_addAToBc, SYM(addAToBc), b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+33, b_+34); A = E;
  CYC(b_+34, b_+36); alu_and(gb, 0x07);
  CYC(b_+36, b_+38); alu_sub(gb, 0x04);
  CYC(b_+38, b_+40); alu_add(gb, 0x48);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); A = E;
  CYC(b_+43, b_+45); alu_and(gb, 0xf8);
  CYC(b_+45, b_+47); A = alu_swap(gb, A);
  CYC(b_+47, b_+48); alu_rlca(gb);
  CYC(b_+48, b_+50); alu_sub(gb, 0x10);
  CYC(b_+50, b_+52); alu_add(gb, 0x48);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  RET(b_+53); return;
}
