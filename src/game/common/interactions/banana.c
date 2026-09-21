#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t banana_jump_table(GB *gb) {
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

static void banana_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0, 0x12, 0x13, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0, 0x12, 0x13, false);
  burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0, 0x14, 0x15, false); ret_effect(gb);
}

void interactionCodec0__afterCall48c6_hook(GB *gb) {
  BASE(interactionCodec0);
  CYC(b_+14, b_+17); objectSetVisible80_hook(gb);
}

void interactionCodec0__state0_hook(GB *gb) {
  BASE(interactionCodec0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  interactionCodec0__afterCall48c6_hook(gb);
}

void interactionCodec0__updatePosition_hook(GB *gb) {
  BASE(interactionCodec0);
  CYC(b_+64, b_+65); push_effect(gb, HL);
  CYC(b_+65, b_+68); SET_HL(b_+80);
  CYC(b_+68, b_+69); banana_add_a_to_hl(gb, b_+69);
  CYC(b_+69, b_+71); B = 0;
  CYC(b_+71, b_+72); C = mem_rd(gb, HL);
  CYC(b_+72, b_+73); SET_HL(pop_effect(gb));
  CYC(b_+73, b_+76); objectTakePositionWithOffset_hook(gb);
}

void interactionCodec0__state1_hook(GB *gb) {
  BASE(interactionCodec0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CYC(b_+20, b_+22); A = OBJ_ENABLED;
  CALL_C(b_+22, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+25);
  CYC(b_+25, b_+27); L = OBJ_ID;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_cp(gb, 0x11);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+33); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+35); E = 0x48;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); L = OBJ_DIRECTION;
  CYC(b_+38, b_+39); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  if (F & FZ) {
    CYCT(b_+40, b_+42); interactionCodec0__updatePosition_hook(gb);
    return;
  }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); push_effect(gb, AF);
  CYC(b_+44, b_+47); SET_HL(b_+76);
  CYC(b_+47, b_+48); banana_add_a_to_hl(gb, b_+48);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+51); E = 0x5a;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); SET_AF(pop_effect(gb));
  CALL_C(b_+53, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+56);
  CYC(b_+56, b_+58); A = OBJ_ENABLED;
  CALL_C(b_+58, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+61);
  CYC(b_+61, b_+63); L = OBJ_DIRECTION;
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  TAIL(interactionCodec0__updatePosition);
}

void interactionCodec0_hook(GB *gb) {
  BASE(interactionCodec0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (banana_jump_table(gb));
    if (jt_ == b_+8) { interactionCodec0__state0_hook(gb); return; }
    else if (jt_ == b_+17) { interactionCodec0__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
