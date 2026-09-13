#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t shovel_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void shovel_parent_add_double_index(GB *gb) {
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void parentItemCode_shovel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f9f, 0x4fa1); E = 0x04;
  CYC(0x4fa1, 0x4fa2); A = mem_rd(gb, DE);
  CYC(0x4fa2, 0x4fa3); push_effect(gb, 0x4fa3);
  switch (shovel_parent_jump_table(gb)) {
    case 0x4fa7: goto state0;
    case 0x4fb0: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x4fa7, checkLinkOnGround_hook, 0x54c4, 0x4faa);
  if (!(F & FZ)) { CYCT(0x4faa, 0x4fad); clearParentItem_hook(gb); return; }
  CYC(0x4faa, 0x4fad);
  CYC(0x4fad, 0x4fb0); parentItemLoadAnimationAndIncState_hook(gb); return;

state1:
  CALL_C(0x4fb0, specialObjectAnimate_optimized_hook, 0x441e, 0x4fb3);
  CYC(0x4fb3, 0x4fb5); E = 0x21;
  CYC(0x4fb5, 0x4fb6); A = mem_rd(gb, DE);
  CYC(0x4fb6, 0x4fb8); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4fb8, 0x4fbb); clearParentItem_hook(gb); return; }
  CYC(0x4fb8, 0x4fbb);
  CYC(0x4fbb, 0x4fbc); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4fbc, 0x4fbd); ret_effect(gb); return; }
  CYC(0x4fbc, 0x4fbd);
  CYC(0x4fbd, 0x4fbe); mem_wr(gb, DE, A);
  CALL_C(0x4fbe, itemCreateChildIfDoesntExistAlready_hook, 0x53d4, 0x4fc1);
  CYC(0x4fc1, 0x4fc2); push_effect(gb, HL);
  CYC(0x4fc2, 0x4fc4); L = 0x08;
  CYC(0x4fc4, 0x4fc5); A = mem_rd(gb, HL);
  CYC(0x4fc5, 0x4fc8); SET_HL(0x4fd5);
  CYC(0x4fc8, 0x4fc9); push_effect(gb, 0x4fc9); shovel_parent_add_double_index(gb);
  CYC(0x4fc9, 0x4fca); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4fca, 0x4fcb); C = mem_rd(gb, HL);
  CYC(0x4fcb, 0x4fcc); SET_HL(pop_effect(gb));
  CYC(0x4fcc, 0x4fce); L = 0x0b;
  CYC(0x4fce, 0x4fcf); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4fcf, 0x4fd0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4fd0, 0x4fd1); L = alu_inc8(gb, L);
  CYC(0x4fd1, 0x4fd2); A = mem_rd(gb, HL);
  CYC(0x4fd2, 0x4fd3); alu_add(gb, C);
  CYC(0x4fd3, 0x4fd4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4fd4, 0x4fd5); ret_effect(gb);
}
