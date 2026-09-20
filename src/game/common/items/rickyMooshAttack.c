#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(itemCode28), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(itemCode28), (from), (to), true)

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
  BASE(itemCode28);
  CYC(b_+31, b_+34); A = W8(w1Companion_id);
  CYC(b_+34, b_+36); alu_cp(gb, 0x0b);
  CYC(b_+36, b_+39); SET_HL(b_+68);
  if (!(F & FZ)) {
    CYCT(b_+39, b_+41);
  } else {
    CYC(b_+39, b_+41);
    CYC(b_+41, b_+44); A = W8(w1Companion_direction);
    CYC(b_+44, b_+45); alu_add(gb, A);
    CYC(b_+45, b_+48); SET_HL(b_+52);
    CYC(b_+48, b_+49); push_effect(gb, b_+49); ricky_moosh_add_double_index(gb);
  }
  CYC(b_+49, b_+52); itemInitializeFromLinkPosition_hook(gb);
}

static void ricky_moosh_try_to_break_tiles(GB *gb, uint16_t sp0_) {
  BASE(itemCode28);
  CYC(b_+72, b_+75); SET_HL(b_+119);
  CYC(b_+75, b_+78); A = W8(w1Companion_id);
  CYC(b_+78, b_+80); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(b_+80, b_+82);
  } else {
    CYC(b_+80, b_+82);
    CYC(b_+82, b_+85); SET_HL(b_+128);
  }

next_tile:
  CYC(b_+85, b_+87); E = 0x0b;
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  CYC(b_+88, b_+89); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+89, b_+90); B = A;
  CYC(b_+90, b_+91); SET_HL(HL + 1);
  CYC(b_+91, b_+93); E = 0x0d;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+95); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+95, b_+96); C = A;
  CYC(b_+96, b_+97); SET_HL(HL + 1);
  CYC(b_+97, b_+98); push_effect(gb, HL);
  CYC(b_+98, b_+101); A = W8(w1Companion_id);
  CYC(b_+101, b_+103); alu_cp(gb, 0x0b);
  CYC(b_+103, b_+105); A = 0x0f;
  if (F & FZ) {
    CYCT(b_+105, b_+107);
  } else {
    CYC(b_+105, b_+107);
    CYC(b_+107, b_+109); A = 0x11;
  }
  CALL_C(b_+109, tryToBreakTile_hook, SYM(tryToBreakTile), b_+112);
  CYC(b_+112, b_+113); SET_HL(pop_effect(gb));
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);
  CYC(b_+114, b_+116); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+116, b_+118);
    goto next_tile;
  }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+119); ret_effect(gb);
}

void itemCode28_hook(GB *gb) {
  BASE(itemCode28);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    CYC(b_+18, b_+21); push_effect(gb, b_+21); ricky_moosh_calculate_position(gb);
    CYC(b_+21, b_+24); push_effect(gb, b_+24); ricky_moosh_try_to_break_tiles(gb, sp0_);
    CALL_C(b_+24, itemDecCounter1_hook, SYM(itemDecCounter1), b_+27);
    if (!(F & FZ)) { CYCT(b_+27, b_+28); ret_effect(gb); return; }
    CYC(b_+27, b_+28);
    CYC(b_+28, b_+31); itemDelete_hook(gb);
    return;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, itemIncState_hook, SYM(itemIncState), b_+9);
  CYC(b_+9, b_+11); L = 0x06;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x14);
  CALL_C(b_+13, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+16);
  CYC(b_+16, b_+18);
  ricky_moosh_calculate_position(gb);
}
