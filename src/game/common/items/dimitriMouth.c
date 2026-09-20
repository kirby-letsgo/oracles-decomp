#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void dimitri_mouth_add_double_index(GB *gb) {
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void dimitri_mouth_calculate_position(GB *gb, uint16_t sp0_) {
  BASE(itemCode2b);
  CYC(b_+47, b_+50); A = W8(w1Companion_direction);
  CYC(b_+50, b_+53); SET_HL(b_+63);
  CYC(b_+53, b_+54); push_effect(gb, b_+54); dimitri_mouth_add_double_index(gb);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+55, b_+56); C = mem_rd(gb, HL);
  CYC(b_+56, b_+57); B = A;
  CYC(b_+57, b_+60); SET_HL(w1Companion_yh);
  CYC(b_+60, b_+63); objectTakePositionWithOffset_hook(gb);
}

void itemCode2b_hook(GB *gb) {
  BASE(itemCode2b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CALL_C(b_+6, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+9);
    CALL_C(b_+9, itemIncState_hook, SYM(itemIncState), b_+12);
    CYC(b_+12, b_+14); L = 0x06;
    CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0c);
  }
  CYC(b_+16, b_+19); push_effect(gb, b_+19); dimitri_mouth_calculate_position(gb, sp0_);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0x2a;
  CYC(b_+22, b_+24); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+24, b_+26);
    goto swallow;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x12;
  CALL_C(b_+28, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+31);
  if (F & FC) {
    CYCT(b_+31, b_+33);
    goto swallow;
  }
  CYC(b_+31, b_+33);
  CALL_C(b_+33, itemDecCounter1_hook, SYM(itemDecCounter1), b_+36);
  if (F & FZ) {
    CYCT(b_+36, b_+38);
  } else {
    CYC(b_+36, b_+38);
    CYC(b_+38, b_+39); ret_effect(gb);
    return;
  }
  goto delete_self;

swallow:
  CYC(b_+39, b_+41); A = 0x01;
  CYC(b_+41, b_+44); W8(w1Companion_var35) = A;

delete_self:
  CYC(b_+44, b_+47); itemDelete_hook(gb);
}
