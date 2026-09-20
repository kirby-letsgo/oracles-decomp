#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t boomerang_parent_jump_table(GB *gb) {
  burn_rom(gb, 0, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 0x0004, 0x0006, false); burn_rom(gb, 0, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 0x0004, 0x0006, true);
  burn_rom(gb, 0, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0, 0x000a, 0x000b, false);
  return HL;
}

void parentItemCode_boomerang_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fdd, 0x4fdf); E = 0x04;
  CYC(0x4fdf, 0x4fe0); A = mem_rd(gb, DE);
  CYC(0x4fe0, 0x4fe1); push_effect(gb, 0x4fe1);
  switch (boomerang_parent_jump_table(gb)) {
    case 0x4fe5: break;
    case 0x5024: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x4fe5, isLinkUnderwater_hook, 0x54d2, 0x4fe8);
  if (!(F & FZ)) { CYCT(0x4fe8, 0x4feb); clearParentItem_hook(gb); return; }
  CYC(0x4fe8, 0x4feb);
  CYC(0x4feb, 0x4fee); A = W8(w1ParentItem2_id);
  CYC(0x4fee, 0x4ff0); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(0x4ff0, 0x4ff3); clearParentItem_hook(gb); return; }
  CYC(0x4ff0, 0x4ff3);
  CYC(0x4ff3, 0x4ff6); A = W8(wLinkSwimmingState);
  CYC(0x4ff6, 0x4ff7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4ff7, 0x4ffa); clearParentItem_hook(gb); return; }
  CYC(0x4ff7, 0x4ffa);
  CALL_C(0x4ffa, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4ffd);
  CYC(0x4ffd, 0x4fff); A = 1;
  CYC(0x4fff, 0x5001); E = 0x04;
  CYC(0x5001, 0x5002); mem_wr(gb, DE, A);
  CYC(0x5002, 0x5003); A = alu_dec8(gb, A);
  CYC(0x5003, 0x5004); C = A;
  CYC(0x5004, 0x5006); E = 1;
  CYC(0x5006, 0x5008); A = mem_rd(gb, DE); B = A;
  CYC(0x5008, 0x500a); E = 1;
  CALL_C(0x500a, itemCreateChildWithID_hook, 0x53e3, 0x500d);
  if (F & FC) { CYCT(0x500d, 0x5010); clearParentItem_hook(gb); return; }
  CYC(0x500d, 0x5010);
  CYC(0x5010, 0x5013); A = W8(wLinkAngle);
  CYC(0x5013, 0x5015); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(0x5015, 0x5017);
  else {
    CYC(0x5015, 0x5017);
    CYC(0x5017, 0x501a); A = W8(w1Link_direction);
    CYC(0x501a, 0x501c); A = alu_swap(gb, A);
    CYC(0x501c, 0x501d); alu_rrca(gb);
  }
  CYC(0x501d, 0x501f); L = 0x09;
  CYC(0x501f, 0x5020); mem_wr(gb, HL, A);
  CYC(0x5020, 0x5022); L = 0x34;
  CYC(0x5022, 0x5024); mem_wr(gb, HL, A); ret_effect(gb); return;

state1:
  CYC(0x5024, 0x5026); E = 0x21;
  CYC(0x5026, 0x5027); A = mem_rd(gb, DE);
  CYC(0x5027, 0x5028); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x5028, 0x502b); specialObjectAnimate_optimized_hook(gb); return; }
  CYC(0x5028, 0x502b); CYC(0x502b, 0x502e); clearParentItem_hook(gb);
}
