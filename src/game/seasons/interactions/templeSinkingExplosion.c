#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/templeSinkingExplosion.s, bank $09.

static void rst_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Places the explosion at its subid's entry of the stage (wTmpcfc0+$13) table in table_7cd8,
// each coordinate jittered by -2..1.
void s_func_7cb3_hook(GB *gb) {
  BASE(func_7cb3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x13);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x7f);
  CYC(b_+6, b_+9); SET_HL(SYM(table_7cd8));
  CYC(b_+9, b_+10); rst_add_double_index(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+15); E = 0x42; // Interaction.subid
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); rst_add_double_index(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+20); E = 0x4b; // Interaction.yh
  CALL_C(b_+20, s_func_7ccd_hook, SYM(func_7ccd), b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); E = 0x4d; // Interaction.xh
  TAIL_S(func_7ccd);
}

// (de) = a plus a random -2..1.
void s_func_7ccd_hook(GB *gb) {
  BASE(func_7ccd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getRandomNumber_hook, SYM(getRandomNumber), b_+4);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+8); alu_sub(gb, 0x02);
  CYC(b_+8, b_+9); alu_add(gb, B);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

static void templeSinkingExplosion_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_TEMPLE_SINKING_EXPLOSION
void s_interactionCode86_hook(GB *gb) {
  BASE(interactionCode86);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ANIM_POINTER;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  CYC(b_+23, b_+24); L = A;
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+26); templeSinkingExplosion_d_add_a_to_hl(gb, b_+26);
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_ANIM_POINTER;
  CYC(b_+28, b_+29); A = L;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); E = alu_inc8(gb, E);
  CYC(b_+31, b_+32); A = H;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CALL_C(b_+33, s_func_7cb3_hook, SYM(func_7cb3), b_+36);
  CYC(b_+36, b_+39);
  TAIL(objectSetVisible81);
state1:
  CYC(b_+39, b_+42); SET_HL(wTmpcfc0 + 0x13);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+44); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+48); A = alu_dec8(gb, A);
  CYC(b_+48, b_+50); alu_and(gb, 0x7f);
  CYC(b_+50, b_+51); B = A;
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+56); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+56, b_+58); goto L_7c97; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, B);
  CALL_C(b_+59, s_func_7cb3_hook, SYM(func_7cb3), b_+62);
  CYC(b_+62, b_+64);
  goto L_7c9e;
L_7c97:
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); A = alu_inc8(gb, A);
  if (F & FZ) CALL_C_CC(b_+68, s_func_7cb3_hook, SYM(func_7cb3), b_+71);
  else CYC(b_+68, b_+71);
L_7c9e:
  CALL_C(b_+71, s_interactionAnimate, SYM(interactionAnimate), b_+74);
  CYC(b_+74, b_+76); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+79); alu_and(gb, 0x01);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+83); A = mem_rd(gb, wFrameCounter);
  CYC(b_+83, b_+85); alu_and(gb, 0x01);
  CYC(b_+85, b_+86); alu_xor(gb, B);
  if (F & FZ) { CYCT(b_+86, b_+89); TAIL(objectSetInvisible); }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+92);
  TAIL(objectSetVisible);
}

