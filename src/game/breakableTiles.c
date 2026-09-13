#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void makeInteractionForBreakableTile_hook(GB *gb);

void itemMakeInteractionForBreakableTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4809, 0x480a); H = D;
  CYC(0x480a, 0x480c); L = 0x0b;
  CYC(0x480c, 0x480d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x480d, 0x480f); H8(hFF90) = A;
  CYC(0x480f, 0x4810); L = alu_inc8(gb, L);
  CYC(0x4810, 0x4811); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4811, 0x4813); H8(hFF91) = A;
  CYC(0x4813, 0x4815); L = 0x03;
  CYC(0x4815, 0x4816); A = mem_rd(gb, HL);
  makeInteractionForBreakableTile_hook(gb);
}

void makeInteractionForBreakableTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4816, 0x4818); alu_and(gb, 0x1f);
  CYC(0x4818, 0x481a); alu_cp(gb, 0x1f);
  if (F & FZ) {
    CYCT(0x481a, 0x481b); ret_effect(gb); return;
  }
  CYC(0x481a, 0x481b);
  CYC(0x481b, 0x481c); C = A;
  CALL_C(0x481c, getFreeInteractionSlot_hook, 0x3aef, 0x481f);
  if (!(F & FZ)) {
    CYCT(0x481f, 0x4820); ret_effect(gb); return;
  }
  CYC(0x481f, 0x4820);
  CYC(0x4820, 0x4821); A = C;
  CYC(0x4821, 0x4823); alu_and(gb, 0x0f);
  CYC(0x4823, 0x4824); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4824, 0x4825); A = C;
  CYC(0x4825, 0x4827); alu_and(gb, 0x10);
  CYC(0x4827, 0x4829); A = alu_swap(gb, A);
  CYC(0x4829, 0x482a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x482a, 0x482d); A = W8(w1Link_direction);
  CYC(0x482d, 0x482f); L = 0x48;
  CYC(0x482f, 0x4830); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4830, 0x4832); A = alu_swap(gb, A);
  CYC(0x4832, 0x4833); alu_rrca(gb);
  CYC(0x4833, 0x4834); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4834, 0x4835); L = alu_inc8(gb, L);
  CYC(0x4835, 0x4837); A = H8(hFF90);
  CYC(0x4837, 0x4838); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4838, 0x4839); L = alu_inc8(gb, L);
  CYC(0x4839, 0x483b); A = H8(hFF91);
  CYC(0x483b, 0x483c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x483c, 0x483d); ret_effect(gb);
}
