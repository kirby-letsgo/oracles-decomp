#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t punch_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void punch_body(GB *gb, uint16_t sp0_) {
  BASE(itemCode00);
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (punch_jump_table(gb));
    if (jt_ == b_+8) {
      CALL_C(b_+8, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+11);
      CYC(b_+11, b_+13); C = 0xa6;
      CALL_C(b_+13, itemIncState_hook, SYM(itemIncState), b_+16);
      CYC(b_+16, b_+18); L = 0x06;
      CYC(b_+18, b_+20); mem_wr(gb, HL, 0x04);
      CYC(b_+20, b_+22); L = 0x02;
      CYC(b_+22, b_+24); alu_bit(gb, 0, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(b_+24, b_+26);
      } else {
        CYC(b_+24, b_+26);
        CYC(b_+26, b_+28); L = 0x26;
        CYC(b_+28, b_+30); A = 0x06;
        CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(b_+32, b_+33); A = mem_rd(gb, HL);
        CYC(b_+33, b_+35); alu_add(gb, 0xfd);
        CYC(b_+35, b_+36); mem_wr(gb, HL, A);
        CYC(b_+36, b_+38); L = 0x24;
        CYC(b_+38, b_+39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        CALL_C(b_+39, tryBreakTileWithExpertsRing_hook, SYM(tryBreakTileWithExpertsRing), b_+42);
        CYC(b_+42, b_+44); C = 0x6f;
      }
      CYC(b_+44, b_+45); A = C;
      CYC(b_+45, b_+48); playSound_b00_hook(gb); return;
    }
    else if (jt_ == b_+48) {
      CALL_C(b_+48, itemDecCounter1_hook, SYM(itemDecCounter1), b_+51);
      if (F & FZ) {
        CYCT(b_+51, b_+54); itemDelete_hook(gb); return;
      }
      CYC(b_+51, b_+54);
      CYC(b_+54, b_+55); ret_effect(gb); return;
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void itemCode00_hook(GB *gb) {
  punch_body(gb, gb->sp);
}

void itemCode02_hook(GB *gb) {
  punch_body(gb, gb->sp);
}
