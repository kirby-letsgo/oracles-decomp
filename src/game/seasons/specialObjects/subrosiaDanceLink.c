#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/specialObjects/subrosiaDanceLink.s.

static uint16_t subrosiaDanceLink_jump_table(GB *gb) {
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

void s_specialObjectCode_subrosiaDanceLink_hook(GB *gb) {
  BASE(specialObjectCode_subrosiaDanceLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiaDanceLink_jump_table(gb));
    if (jt_ == SYM(subrosiaDance_state0) && hook_is(gb, SYM(subrosiaDance_state0), s_subrosiaDance_state0_hook)) { s_subrosiaDance_state0_hook(gb); return; }
    if (jt_ == SYM(subrosiaDance_state1) && hook_is(gb, SYM(subrosiaDance_state1), s_subrosiaDance_state1_hook)) { s_subrosiaDance_state1_hook(gb); return; }
    if (jt_ == SYM(subrosiaDance_state2) && hook_is(gb, SYM(subrosiaDance_state2), s_subrosiaDance_state2_hook)) { s_subrosiaDance_state2_hook(gb); return; }
    if (jt_ == SYM(subrosiaDance_state3) && hook_is(gb, SYM(subrosiaDance_state3), s_subrosiaDance_state3_hook)) { s_subrosiaDance_state3_hook(gb); return; }
    if (jt_ == SYM(subrosiaDance_state4) && hook_is(gb, SYM(subrosiaDance_state4), s_subrosiaDance_state4_hook)) { s_subrosiaDance_state4_hook(gb); return; }
    if (jt_ == SYM(subrosiaDance_state5) && hook_is(gb, SYM(subrosiaDance_state5), s_subrosiaDance_state5_hook)) { s_subrosiaDance_state5_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_subrosiaDance_state0_hook(GB *gb) {
  BASE(subrosiaDance_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_clearAllParentItems, SYM(clearAllParentItems), b_+3);
  CALL_C(b_+3, s_specialObjectSetOamVariables, SYM(specialObjectSetOamVariables), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x00;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_or(gb, 0x03);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x04;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x01);
  CYC(b_+17, b_+19); L = 0x08;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x02);
  CYC(b_+21, b_+23); L = 0x0b;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x38);
  CYC(b_+25, b_+27); L = 0x0d;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x68);
  CYC(b_+29, b_+31); A = 0x00;
  CALL_C(b_+31, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+34);
  CYC(b_+34, b_+37);
  TAIL(objectSetVisiblec1);
}

void s_subrosiaDance_state1_hook(GB *gb) {
  BASE(subrosiaDance_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_subrosiaDance_checkSpinOrCollapse_hook, SYM(subrosiaDance_checkSpinOrCollapse), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+7); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+7, b_+9); B = 0x00;
  CYC(b_+9, b_+11); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_7d27; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); B = alu_inc8(gb, B);
  CYC(b_+14, b_+16); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto L_7d27; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); B = alu_inc8(gb, B);
  CYC(b_+19, b_+21); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); TAIL_S(subrosiaDance_buttonAPressed); }
  CYC(b_+21, b_+23);
  RET(b_+23); return;
L_7d27:
  CALL_C(b_+24, s_subrosiaDance_storeButtonPress_hook, SYM(subrosiaDance_storeButtonPress), b_+27);
  CYC(b_+27, b_+29); L = 0x04;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+32); L = 0x37;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x00);
  CALL_C(b_+34, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+37);
  CYC(b_+37, b_+38); C = A;
  CYC(b_+38, b_+41); SET_HL(SYM(subrosiaDance_nextTileLookup));
L_7d38:
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+43, b_+45); goto L_7d3f; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); SET_HL(HL + 1);
  CYC(b_+46, b_+48);
  goto L_7d38;
L_7d3f:
  CYC(b_+48, b_+51); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+51, b_+53); alu_and(gb, 0x10);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_7d49; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); A = alu_swap(gb, A);
L_7d49:
  CYC(b_+58, b_+60); alu_and(gb, 0x0f);
  CYC(b_+60, b_+62); E = 0x08;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+65); A = alu_swap(gb, A);
  CYC(b_+65, b_+66); alu_rrca(gb);
  CYC(b_+66, b_+67); E = alu_inc8(gb, E);
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+69); alu_xor(gb, A);
  CYC(b_+69, b_+72);
  TAIL(specialObjectSetAnimation);
}

void s_subrosiaDance_buttonAPressed_hook(GB *gb) {
  BASE(subrosiaDance_buttonAPressed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_subrosiaDance_storeButtonPress_hook, SYM(subrosiaDance_storeButtonPress), b_+3);
  CYC(b_+3, b_+5); L = 0x04;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x03);
  CYC(b_+7, b_+9); L = 0x08;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+16);
  TAIL(specialObjectSetAnimation);
}

void s_subrosiaDance_storeButtonPress_hook(GB *gb) {
  BASE(subrosiaDance_storeButtonPress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+9); mem_wr(gb, wTmpcfc0 + 0x18, A);
  RET(b_+9); return;
}

void s_subrosiaDance_state2_hook(GB *gb) {
  BASE(subrosiaDance_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_subrosiaDance_checkSpinOrCollapse_hook, SYM(subrosiaDance_checkSpinOrCollapse), b_+3);
  CALL_C(b_+3, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+6);
  CALL_C(b_+6, s_subrosiaDance_moveLink_hook, SYM(subrosiaDance_moveLink), b_+9);
  if (F & FC) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = 0x04;
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

void s_subrosiaDance_moveLink_hook(GB *gb) {
  BASE(subrosiaDance_moveLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = 0x0b;
  CYC(b_+3, b_+5); L = 0x38;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+9); E = 0x0d;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x13);
  CYC(b_+14, b_+16); E = 0x10;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, s_objectApplySpeed, SYM(objectApplySpeed), b_+20);
  CALL_C(b_+20, s_fixXYWithinDancingGrid_hook, SYM(fixXYWithinDancingGrid), b_+23);
  CYC(b_+23, b_+25);
  TAIL_S(subrosiaDance_05_7dd7);
}

void s_fixXYWithinDancingGrid_hook(GB *gb) {
  BASE(fixXYWithinDancingGrid);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0b;
  CALL_C(b_+3, s_fixXYWithinDancingGrid_helper_hook, SYM(fixXYWithinDancingGrid_helper), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x0d;
  s_fixXYWithinDancingGrid_helper_hook(gb); return; // falls through
}

void s_fixXYWithinDancingGrid_helper_hook(GB *gb) {
  BASE(fixXYWithinDancingGrid_helper);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17;
  CYC(b_+2, b_+3); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FC)) { CYCT(b_+4, b_+6); goto L_7dd5; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x68;
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
L_7dd5:
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return;
}

void s_subrosiaDance_05_7dd7_hook(GB *gb) {
  BASE(subrosiaDance_05_7dd7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x0b;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = 0x38;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+8, b_+10); goto L_7de3; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); alu_cpl(gb);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
L_7de3:
  CYC(b_+12, b_+13); C = A;
  CYC(b_+13, b_+15); E = 0x0d;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19); E = 0x39;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto L_7df0; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); alu_cpl(gb);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
L_7df0:
  CYC(b_+25, b_+26); alu_add(gb, C);
  if (F & FZ) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); E = 0x37;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_add(gb, B);
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+35); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39);
  TAIL(objectCenterOnTile);
}

void s_subrosiaDance_state3_hook(GB *gb) {
  BASE(subrosiaDance_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_subrosiaDance_checkSpinOrCollapse_hook, SYM(subrosiaDance_checkSpinOrCollapse), b_+3);
  CALL_C(b_+3, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+6);
  CYC(b_+6, b_+8); E = 0x21;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = 0x04;
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}

void s_subrosiaDance_state4_hook(GB *gb) {
  BASE(subrosiaDance_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(specialObjectAnimate);
}

void s_subrosiaDance_state5_hook(GB *gb) {
  BASE(subrosiaDance_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void s_subrosiaDance_checkSpinOrCollapse_hook(GB *gb) {
  BASE(subrosiaDance_checkSpinOrCollapse);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); SET_HL(pop_effect(gb));
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+9); A = 0x05;
  CYC(b_+9, b_+11); B = 0x02;
  if (F & FZ) { CYCT(b_+11, b_+13); goto L_7e23; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = alu_dec8(gb, A);
  CYC(b_+14, b_+16); B = 0x01;
L_7e23:
  CYC(b_+16, b_+18); E = 0x04;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); A = B;
  CALL_C(b_+20, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+23);
  CYC(b_+23, b_+26);
  TAIL(objectSetVisible80);
}

