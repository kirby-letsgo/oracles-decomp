#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static void ricky_moosh_add_double_index(GB *gb) {
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

static void ricky_moosh_calculate_position(GB *gb) {
  CYC(0x5bab, 0x5bae); A = W8(w1Companion_id);
  CYC(0x5bae, 0x5bb0); alu_cp(gb, 0x0b);
  CYC(0x5bb0, 0x5bb3); SET_HL(0x5bd0);
  if (!(F & FZ)) {
    CYCT(0x5bb3, 0x5bb5);
  } else {
    CYC(0x5bb3, 0x5bb5);
    CYC(0x5bb5, 0x5bb8); A = W8(w1Companion_direction);
    CYC(0x5bb8, 0x5bb9); alu_add(gb, A);
    CYC(0x5bb9, 0x5bbc); SET_HL(0x5bc0);
    CYC(0x5bbc, 0x5bbd); push_effect(gb, 0x5bbd); ricky_moosh_add_double_index(gb);
  }
  CYC(0x5bbd, 0x5bc0); itemInitializeFromLinkPosition_hook(gb);
}

static void ricky_moosh_try_to_break_tiles(GB *gb, uint16_t sp0_) {
  CYC(0x5bd4, 0x5bd7); SET_HL(0x5c03);
  CYC(0x5bd7, 0x5bda); A = W8(w1Companion_id);
  CYC(0x5bda, 0x5bdc); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(0x5bdc, 0x5bde);
  } else {
    CYC(0x5bdc, 0x5bde);
    CYC(0x5bde, 0x5be1); SET_HL(0x5c0c);
  }

next_tile:
  CYC(0x5be1, 0x5be3); E = 0x0b;
  CYC(0x5be3, 0x5be4); A = mem_rd(gb, DE);
  CYC(0x5be4, 0x5be5); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5be5, 0x5be6); B = A;
  CYC(0x5be6, 0x5be7); SET_HL(HL + 1);
  CYC(0x5be7, 0x5be9); E = 0x0d;
  CYC(0x5be9, 0x5bea); A = mem_rd(gb, DE);
  CYC(0x5bea, 0x5beb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5beb, 0x5bec); C = A;
  CYC(0x5bec, 0x5bed); SET_HL(HL + 1);
  CYC(0x5bed, 0x5bee); push_effect(gb, HL);
  CYC(0x5bee, 0x5bf1); A = W8(w1Companion_id);
  CYC(0x5bf1, 0x5bf3); alu_cp(gb, 0x0b);
  CYC(0x5bf3, 0x5bf5); A = 0x0f;
  if (F & FZ) {
    CYCT(0x5bf5, 0x5bf7);
  } else {
    CYC(0x5bf5, 0x5bf7);
    CYC(0x5bf7, 0x5bf9); A = 0x11;
  }
  CALL_C(0x5bf9, tryToBreakTile_hook, 0x2bf6, 0x5bfc);
  CYC(0x5bfc, 0x5bfd); SET_HL(pop_effect(gb));
  CYC(0x5bfd, 0x5bfe); A = mem_rd(gb, HL);
  CYC(0x5bfe, 0x5c00); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x5c00, 0x5c02);
    goto next_tile;
  }
  CYC(0x5c00, 0x5c02);
  CYC(0x5c02, 0x5c03); ret_effect(gb);
}

void itemCode28_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b8c, 0x5b8e); E = 0x04;
  CYC(0x5b8e, 0x5b8f); A = mem_rd(gb, DE);
  CYC(0x5b8f, 0x5b90); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5b90, 0x5b92);
    CYC(0x5b9e, 0x5ba1); push_effect(gb, 0x5ba1); ricky_moosh_calculate_position(gb);
    CYC(0x5ba1, 0x5ba4); push_effect(gb, 0x5ba4); ricky_moosh_try_to_break_tiles(gb, sp0_);
    CALL_C(0x5ba4, itemDecCounter1_hook, 0x23d6, 0x5ba7);
    if (!(F & FZ)) { CYCT(0x5ba7, 0x5ba8); ret_effect(gb); return; }
    CYC(0x5ba7, 0x5ba8);
    CYC(0x5ba8, 0x5bab); itemDelete_hook(gb);
    return;
  }
  CYC(0x5b90, 0x5b92);
  CALL_C(0x5b92, itemIncState_hook, 0x23ea, 0x5b95);
  CYC(0x5b95, 0x5b97); L = 0x06;
  CYC(0x5b97, 0x5b99); mem_wr(gb, HL, 0x14);
  CALL_C(0x5b99, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5b9c);
  CYC(0x5b9c, 0x5b9e);
  ricky_moosh_calculate_position(gb);
}
