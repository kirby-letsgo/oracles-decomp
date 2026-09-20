#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionLoadTreasureData_hook(GB *gb) {
  BASE(interactionLoadTreasureData);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = 0x70;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9); SET_HL(SYM(treasureObjectData));
  for (;;) {
    CALL_C(b_+9, multiplyABy4_hook, SYM(multiplyABy4), b_+12);
    CYC(b_+12, b_+13); alu_add_hl(gb, BC);
    CYC(b_+13, b_+15); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+15, b_+17);
      break;
    }
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+18); SET_HL(HL + 1);
    CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+19, b_+20); H = mem_rd(gb, HL);
    CYC(b_+20, b_+21); L = A;
    CYC(b_+21, b_+23); E = 0x43;
    CYC(b_+23, b_+24); A = mem_rd(gb, DE);
    CYC(b_+24, b_+26);
  }
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); A = alu_swap(gb, A);
  CYC(b_+30, b_+32); alu_and(gb, 0x07);
  CYC(b_+32, b_+34); E = 0x71;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+38); alu_and(gb, 0x07);
  CYC(b_+38, b_+39); E = alu_inc8(gb, E);
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+41); A = B;
  CYC(b_+41, b_+43); alu_and(gb, 0x08);
  CYC(b_+43, b_+44); E = alu_inc8(gb, E);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+54); E = 0x42;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+56); ret_effect(gb);
}
