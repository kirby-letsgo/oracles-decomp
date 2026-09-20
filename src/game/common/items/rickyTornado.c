#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(itemCode2a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(itemCode2a), (from), (to), true)

static uint16_t ricky_tornado_jump_table(GB *gb) {
  burn_rom(gb, 0, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0004, 0x0006, false);
    burn_rom(gb, 0, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0, 0x000a, 0x000b, false);
  return HL;
}

static void ricky_tornado_add_double_index(GB *gb) {
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

void itemCode2a_hook(GB *gb) {
  BASE(itemCode2a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ricky_tornado_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+60) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

  CALL_C(b_+8, itemIncState_hook, SYM(itemIncState), b_+11);
  CYC(b_+11, b_+13); L = 0x10;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x78);
  CYC(b_+15, b_+18); A = W8(w1Companion_direction);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+22); alu_rrca(gb);
  CYC(b_+22, b_+24); L = 0x09;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+26); A = C;
  CYC(b_+26, b_+29); SET_HL(b_+52);
  CYC(b_+29, b_+30); push_effect(gb, b_+30); ricky_tornado_add_double_index(gb);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+32); C = mem_rd(gb, HL);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+36); SET_HL(w1Companion_yh);
  CALL_C(b_+36, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+39);
  CYC(b_+39, b_+41); alu_sub(gb, 0x02);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CALL_C(b_+42, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+45);
  CYC(b_+45, b_+46); alu_xor(gb, A);
  CALL_C(b_+46, itemSetAnimation_hook, SYM(itemSetAnimation), b_+49);
  CYC(b_+49, b_+52); objectSetVisiblec1_hook(gb);
  return;

state1:
  CALL_C(b_+60, objectApplySpeed_hook, SYM(objectApplySpeed), b_+63);
  CYC(b_+63, b_+65); A = 0x01;
  CALL_C(b_+65, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+68);
  CALL_C(b_+68, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+71);
  CYC(b_+71, b_+73); alu_and(gb, 0x0f);
  CYC(b_+73, b_+75); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+75, b_+78); itemDelete_hook(gb); return;
  }
  CYC(b_+75, b_+78);
  CYC(b_+78, SYM(itemCode29)); itemAnimate_hook(gb);
}
