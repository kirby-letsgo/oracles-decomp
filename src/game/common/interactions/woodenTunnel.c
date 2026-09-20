#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode98), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode98), (from), (to), true)

static uint16_t wooden_tunnel_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false); return HL;
}

void wooden_tunnel_set_solidity_hook(GB *gb) {
  BASE(interactionCode98);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+64, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+67);
  CYC(b_+67, b_+69); H = 0xce;
  CYC(b_+69, b_+70); L = A;
  CYC(b_+70, b_+71); mem_wr(gb, HL, C);
  CYC(b_+71, SYM(interactionCode9f)); ret_effect(gb);
}

void interactionCode98__setSolidity_hook(GB *gb) { wooden_tunnel_set_solidity_hook(gb); }
void interactionCode98__makeSolid_hook(GB *gb) {
  BASE(interactionCode98);
  CYC(b_+62, b_+64); C = 0x0f;
  wooden_tunnel_set_solidity_hook(gb);
}

void interactionCode98__state0_hook(GB *gb) {
  BASE(interactionCode98);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); A = 7;
  CALL_C(b_+16, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+19);
  CYC(b_+19, b_+21); E = 0x42;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24);
  CALL_C(b_+24, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+27);
  CYC(b_+27, b_+30); objectSetVisible81_hook(gb);
}

void interactionCode98__state1_hook(GB *gb) {
  BASE(interactionCode98);
  CYC(b_+30, b_+33); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); interactionCode98__makeSolid_hook(gb); return; }
  CYC(b_+34, b_+36); CYC(b_+36, b_+39); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+39, b_+41); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); interactionCode98__makeSolid_hook(gb); return; }
  CYC(b_+41, b_+43); CYC(b_+43, b_+46); A = mem_rd(gb, w1ReservedItemC_enabled);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); interactionCode98__makeSolid_hook(gb); return; }
  CYC(b_+47, b_+49); CYC(b_+49, b_+51); E = 0x42;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+54); alu_cp(gb, 2);
  CYC(b_+54, b_+56); C = 0x11;
  if (F & FC) { CYCT(b_+56, b_+58); wooden_tunnel_set_solidity_hook(gb); return; }
  CYC(b_+56, b_+58); CYC(b_+58, b_+60); C = 0x19;
  CYC(b_+60, b_+62); wooden_tunnel_set_solidity_hook(gb);
}

void interactionCode98_hook(GB *gb) {
  BASE(interactionCode98);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (wooden_tunnel_jump_table(gb));
    if (jt_ == b_+8) { interactionCode98__state0_hook(gb); return; }
    else if (jt_ == b_+30) { interactionCode98__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
