#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x5b00, 0x5b02); E = 0x04;
  CYC(0x5b02, 0x5b03); A = mem_rd(gb, DE);
  CYC(0x5b03, 0x5b04); push_effect(gb, 0x5b04);
  switch (ricky_tornado_jump_table(gb)) {
    case 0x5b08: break;
    case 0x5b3c: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

  CALL_C(0x5b08, itemIncState_hook, 0x23ea, 0x5b0b);
  CYC(0x5b0b, 0x5b0d); L = 0x10;
  CYC(0x5b0d, 0x5b0f); mem_wr(gb, HL, 0x78);
  CYC(0x5b0f, 0x5b12); A = W8(w1Companion_direction);
  CYC(0x5b12, 0x5b13); C = A;
  CYC(0x5b13, 0x5b15); A = alu_swap(gb, A);
  CYC(0x5b15, 0x5b16); alu_rrca(gb);
  CYC(0x5b16, 0x5b18); L = 0x09;
  CYC(0x5b18, 0x5b19); mem_wr(gb, HL, A);
  CYC(0x5b19, 0x5b1a); A = C;
  CYC(0x5b1a, 0x5b1d); SET_HL(0x5b34);
  CYC(0x5b1d, 0x5b1e); push_effect(gb, 0x5b1e); ricky_tornado_add_double_index(gb);
  CYC(0x5b1e, 0x5b1f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5b1f, 0x5b20); C = mem_rd(gb, HL);
  CYC(0x5b20, 0x5b21); B = A;
  CYC(0x5b21, 0x5b24); SET_HL(w1Companion_yh);
  CALL_C(0x5b24, objectTakePositionWithOffset_hook, 0x2277, 0x5b27);
  CYC(0x5b27, 0x5b29); alu_sub(gb, 0x02);
  CYC(0x5b29, 0x5b2a); mem_wr(gb, DE, A);
  CALL_C(0x5b2a, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5b2d);
  CYC(0x5b2d, 0x5b2e); alu_xor(gb, A);
  CALL_C(0x5b2e, itemSetAnimation_hook, 0x49e2, 0x5b31);
  CYC(0x5b31, 0x5b34); objectSetVisiblec1_hook(gb);
  return;

state1:
  CALL_C(0x5b3c, objectApplySpeed_hook, 0x201d, 0x5b3f);
  CYC(0x5b3f, 0x5b41); A = 0x01;
  CALL_C(0x5b41, itemTryToBreakTile_hook, 0x2bef, 0x5b44);
  CALL_C(0x5b44, objectGetTileCollisions_hook, 0x14ad, 0x5b47);
  CYC(0x5b47, 0x5b49); alu_and(gb, 0x0f);
  CYC(0x5b49, 0x5b4b); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x5b4b, 0x5b4e); itemDelete_hook(gb); return;
  }
  CYC(0x5b4b, 0x5b4e);
  CYC(0x5b4e, 0x5b51); itemAnimate_hook(gb);
}
