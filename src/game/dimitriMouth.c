#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

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
  CYC(0x517c, 0x517f); A = W8(w1Companion_direction);
  CYC(0x517f, 0x5182); SET_HL(0x518c);
  CYC(0x5182, 0x5183); push_effect(gb, 0x5183); dimitri_mouth_add_double_index(gb);
  CYC(0x5183, 0x5184); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5184, 0x5185); C = mem_rd(gb, HL);
  CYC(0x5185, 0x5186); B = A;
  CYC(0x5186, 0x5189); SET_HL(w1Companion_yh);
  CYC(0x5189, 0x518c); objectTakePositionWithOffset_hook(gb);
}

void itemCode2b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x514d, 0x514f); E = 0x04;
  CYC(0x514f, 0x5150); A = mem_rd(gb, DE);
  CYC(0x5150, 0x5151); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5151, 0x5153);
  } else {
    CYC(0x5151, 0x5153);
    CALL_C(0x5153, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5156);
    CALL_C(0x5156, itemIncState_hook, 0x23ea, 0x5159);
    CYC(0x5159, 0x515b); L = 0x06;
    CYC(0x515b, 0x515d); mem_wr(gb, HL, 0x0c);
  }
  CYC(0x515d, 0x5160); push_effect(gb, 0x5160); dimitri_mouth_calculate_position(gb, sp0_);
  CYC(0x5160, 0x5161); H = D;
  CYC(0x5161, 0x5163); L = 0x2a;
  CYC(0x5163, 0x5165); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5165, 0x5167);
    goto swallow;
  }
  CYC(0x5165, 0x5167);
  CYC(0x5167, 0x5169); A = 0x12;
  CALL_C(0x5169, itemTryToBreakTile_hook, 0x2bef, 0x516c);
  if (F & FC) {
    CYCT(0x516c, 0x516e);
    goto swallow;
  }
  CYC(0x516c, 0x516e);
  CALL_C(0x516e, itemDecCounter1_hook, 0x23d6, 0x5171);
  if (F & FZ) {
    CYCT(0x5171, 0x5173);
  } else {
    CYC(0x5171, 0x5173);
    CYC(0x5173, 0x5174); ret_effect(gb);
    return;
  }
  goto delete_self;

swallow:
  CYC(0x5174, 0x5176); A = 0x01;
  CYC(0x5176, 0x5179); W8(w1Companion_var35) = A;

delete_self:
  CYC(0x5179, 0x517c); itemDelete_hook(gb);
}
