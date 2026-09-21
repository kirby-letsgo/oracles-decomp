#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void objectCreateExclamationMark_body_hook(GB *gb) {
  BASE(objectCreateExclamationMark_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  CALL_C(b_+2, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x9f);
  CYC(b_+8, b_+10); L = 0x46;
  CYC(b_+10, b_+12); A = H8(hFF8B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CALL_C(b_+13, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+16);
  CYC(b_+16, b_+17); push_effect(gb, HL);
  CYC(b_+17, b_+19); A = 0x50;
  CALL_ROM(b_+19, SYM(playSound_b00));
  CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  CYC(b_+23, b_+24); ret_effect(gb);
}

void objectCreateFloatingImage_hook(GB *gb) {
  BASE(objectCreateFloatingImage);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xa0);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); A = H8(hFF8D);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+12); A = H8(hFF8B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16); TAIL(objectCopyPositionWithOffset);
}

static uint16_t exclamation_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 4, 6, true);
  }
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void exclamation_mark_after_405b(GB *gb) {
  BASE(interactionCode9f);
  CYC(b_+19, b_+22); TAIL(objectSetVisible80);
}

void interactionCode9f__state0_hook(GB *gb) {
  BASE(interactionCode9f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x40;
  CYC(b_+14, b_+16); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  exclamation_mark_after_405b(gb);
}

void interactionCode9f__state1_hook(GB *gb) {
  BASE(interactionCode9f);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0x46;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+27, b_+30); interactionAnimate_hook(gb);
    return;
  }
  CYC(b_+27, b_+30);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+31, b_+34); interactionAnimate_hook(gb);
    return;
  }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+37); TAIL(interactionDelete);
}

void interactionCode9f_hook(GB *gb) {
  BASE(interactionCode9f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (exclamation_jump_table(gb));
    if (jt_ == b_+8) { interactionCode9f__state0_hook(gb); return; }
    else if (jt_ == b_+22) { interactionCode9f__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
