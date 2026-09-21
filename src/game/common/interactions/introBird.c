#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t intro_bird_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void intro_bird_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x12, 0x13, false);
    burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x14, 0x15, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0, 0x12, 0x13, true); ret_effect(gb);
  }
}

void interactionCoded3__initializePositionAndCounter1_hook(GB *gb) {
  BASE(interactionCoded3);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+49); alu_add(gb, B);
  CYC(b_+49, b_+52); SET_HL(b_+157);
  CYC(b_+52, b_+53); push_effect(gb, b_+53); intro_bird_add_a_to_hl(gb);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+54, b_+55); B = mem_rd(gb, HL);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); C = mem_rd(gb, HL);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = 0x77;
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, b_+63); L = 0x4b;
  CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);
  CYC(b_+65, b_+66); mem_wr(gb, HL, B);
  CYC(b_+66, b_+68); L = 0x46;
  CYC(b_+68, b_+69); mem_wr(gb, HL, C);
  CYC(b_+69, b_+70); ret_effect(gb);
}

void interactionCoded3__afterCall4cd0_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = 0x47;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 45);
  CYC(b_+19, b_+21); L = 0x42;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); B = 0;
  CYC(b_+24, b_+26); C = 0x1a;
  CYC(b_+26, b_+28); alu_cp(gb, 4);
  if (F & FC) CYCT(b_+28, b_+30);
  else {
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+31); B = alu_inc8(gb, B);
    CYC(b_+31, b_+33); C = 6;
  }
  CYC(b_+33, b_+35); L = 0x49;
  CYC(b_+35, b_+36); mem_wr(gb, HL, C);
  CYC(b_+36, b_+38); L = 0x50;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x32);
  CYC(b_+40, b_+41); push_effect(gb, AF);
  CYC(b_+41, b_+42); A = B;
  CALL_C(b_+42, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+45);
  CYC(b_+45, b_+46); SET_AF(pop_effect(gb));
  TAIL(interactionCoded3__initializePositionAndCounter1);
}

void interactionCoded3__state0_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  interactionCoded3__afterCall4cd0_hook(gb);
}

void interactionCoded3__substate0_hook(GB *gb) {
  BASE(interactionCoded3);
  CYC(b_+92, b_+95); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+95, b_+97); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+97, b_+98); ret_effect(gb); return; }
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+101); TAIL(interactionIncSubstate);
}

void interactionCoded3__substate1_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+101, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; }
  CYC(b_+104, b_+105);
  CALL_C(b_+105, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+108);
  CYC(b_+108, b_+111); TAIL(objectSetVisible82);
}

void interactionCoded3__substate2_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+111, b_+113); E = 0x47;
  CYC(b_+113, b_+114); A = mem_rd(gb, DE);
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (F & FZ) CYCT(b_+115, b_+117);
  else {
    CYC(b_+115, b_+117);
    CYC(b_+117, b_+118); A = alu_dec8(gb, A);
    CYC(b_+118, b_+119); mem_wr(gb, DE, A);
  }
  CALL_C(b_+119, interactionAnimate_hook, SYM(interactionAnimate), b_+122);
  CALL_C(b_+122, introObject_applySpeed_hook, SYM(introObject_applySpeed), b_+125);
  CYC(b_+125, b_+127); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+127, b_+128); ret_effect(gb); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+129); H = D;
  CYC(b_+129, b_+131); L = 0x47;
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+133, b_+136); interactionDelete_hook(gb); return; }
  CYC(b_+133, b_+136);
  CYC(b_+136, b_+138); L = 0x45;
  CYC(b_+138, b_+139); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+139, b_+141); L = 0x42;
  CYC(b_+141, b_+142); A = mem_rd(gb, HL);
  CALL_C(b_+142, interactionCoded3__initializePositionAndCounter1_hook, b_+46, b_+145);
  CALL_C(b_+145, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+148);
  CYC(b_+148, b_+150); alu_and(gb, 0x0f);
  CYC(b_+150, b_+151); H = D;
  CYC(b_+151, b_+153); L = 0x46;
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+157); TAIL(objectSetInvisible);
}

void interactionCoded3__state1_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+70, b_+73); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+73, b_+74); B = A;
  CYC(b_+74, b_+76); E = 0x77;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); alu_sub(gb, B);
  CYC(b_+78, b_+79); E = alu_inc8(gb, E);
  CYC(b_+79, b_+81); E = 0x4b;
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+84); E = 0x45;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); push_effect(gb, b_+86);
  do { uint16_t jt_ = (intro_bird_jump_table(gb));
    if (jt_ == b_+92) { interactionCoded3__substate0_hook(gb); return; }
    else if (jt_ == b_+101) { interactionCoded3__substate1_hook(gb); return; }
    else if (jt_ == b_+111) { interactionCoded3__substate2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCoded3_hook(GB *gb) {
  BASE(interactionCoded3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_bird_jump_table(gb));
    if (jt_ == b_+8) { interactionCoded3__state0_hook(gb); return; }
    else if (jt_ == b_+70) { interactionCoded3__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
