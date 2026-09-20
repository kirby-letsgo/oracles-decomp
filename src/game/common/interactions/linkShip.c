#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCoded4), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCoded4), (from), (to), true)

static uint16_t link_ship_jump_table(GB *gb) {
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

static void link_ship_add_a_to_hl(GB *gb) {
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

void interactionCoded4__afterCall4d97_hook(GB *gb) {
  BASE(interactionCoded4);
  CYC(b_+32, b_+35); objectSetVisible82_hook(gb);
}

void interactionCoded4__state0_hook(GB *gb) {
  BASE(interactionCoded4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x42;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+24); alu_and(gb, 0x0f);
  CYC(b_+24, b_+25); alu_add(gb, A);
  CYC(b_+25, b_+26); alu_add(gb, A);
  CYC(b_+26, b_+28); L = 0x46;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CALL_C(b_+29, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+32);
  interactionCoded4__afterCall4d97_hook(gb);
}

void interactionCoded4__ship_hook(GB *gb) {
  BASE(interactionCoded4);
  CYC(b_+50, b_+53); A = mem_rd(gb, wFrameCounter);
  CYC(b_+53, b_+54); B = A;
  CYC(b_+54, b_+56); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+56, b_+57); ret_effect(gb); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+58); A = B;
  CYC(b_+58, b_+60); alu_and(gb, 0xe0);
  CYC(b_+60, b_+62); A = alu_swap(gb, A);
  CYC(b_+62, b_+63); alu_rrca(gb);
  CYC(b_+63, b_+66); SET_HL(b_+72);
  CYC(b_+66, b_+67); push_effect(gb, b_+67); link_ship_add_a_to_hl(gb);
  CYC(b_+67, b_+69); E = 0x4f;
  CYC(b_+69, b_+70); A = mem_rd(gb, HL);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+72); ret_effect(gb);
}

void interactionCoded4__seagull_hook(GB *gb) {
  BASE(interactionCoded4);
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+83); alu_and(gb, 7);
  if (!(F & FZ)) { CYCT(b_+83, b_+84); ret_effect(gb); return; }
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); alu_and(gb, 0x38);
  CYC(b_+87, b_+89); A = alu_swap(gb, A);
  CYC(b_+89, b_+90); alu_rlca(gb);
  CYC(b_+90, b_+93); SET_HL(b_+72);
  CYC(b_+93, b_+94); push_effect(gb, b_+94); link_ship_add_a_to_hl(gb);
  CYC(b_+94, b_+96); E = 0x4f;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, SYM(interactionCoded9)); ret_effect(gb);
}

void interactionCoded4__state1_hook(GB *gb) {
  BASE(interactionCoded4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+35, b_+37); E = 0x42;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); alu_cp(gb, 2);
  if (F & FZ) { CYCT(b_+40, b_+41); ret_effect(gb); return; }
  CYC(b_+40, b_+41);
  CALL_C(b_+41, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+44);
  CYC(b_+44, b_+46); E = 0x42;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); interactionCoded4__seagull_hook(gb); return; }
  CYC(b_+48, b_+50);
  interactionCoded4__ship_hook(gb);
}

void interactionCoded4_hook(GB *gb) {
  BASE(interactionCoded4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_ship_jump_table(gb));
    if (jt_ == b_+8) { interactionCoded4__state0_hook(gb); return; }
    else if (jt_ == b_+35) { interactionCoded4__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
