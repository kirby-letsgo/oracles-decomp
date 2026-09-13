#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static void minecart_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static uint16_t minecart_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void minecart_check_door(GB *gb, uint16_t sp0_) {
  CYC(0x57c3, 0x57c4); A = C;
  CYC(0x57c4, 0x57c6); alu_sub(gb, 0x7c);
  CYC(0x57c6, 0x57c8); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x57c8, 0x57c9); ret_effect(gb); return;
  }
  CYC(0x57c8, 0x57c9);
  CYC(0x57c9, 0x57cb); alu_add(gb, 0x0c);
  CYC(0x57cb, 0x57cc); alu_add(gb, A);
  CYC(0x57cc, 0x57cd); B = A;
  CALL_C(0x57cd, getFreeInteractionSlot_hook, 0x3aef, 0x57d0);
  if (!(F & FZ)) {
    CYCT(0x57d0, 0x57d1); ret_effect(gb); return;
  }
  CYC(0x57d0, 0x57d1);
  CYC(0x57d1, 0x57d3); mem_wr(gb, HL, 0x1e);
  CYC(0x57d3, 0x57d5); L = 0x49;
  CYC(0x57d5, 0x57d6); mem_wr(gb, HL, B);
  CYC(0x57d6, 0x57d8); L = 0x4b;
  CYC(0x57d8, 0x57da); A = H8(hFF8B);
  CYC(0x57da, 0x57db); mem_wr(gb, HL, A);
  CYC(0x57db, 0x57dc); alu_scf(gb);
  CYC(0x57dc, 0x57dd); ret_effect(gb);
}

void minecartCheckCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x570c, getTileAtPosition_hook, 0x1447, 0x570f);
  CYC(0x570f, 0x5710); E = A;
  CYC(0x5710, 0x5711); C = L;
  CYC(0x5711, 0x5712); H = D;
  CYC(0x5712, 0x5714); L = 0x08;
  CYC(0x5714, 0x5715); A = mem_rd(gb, HL);
  CYC(0x5715, 0x5717); A = alu_swap(gb, A);
  CYC(0x5717, 0x571a); SET_HL(0x5783);
  CYC(0x571a, 0x571b); minecart_add_a_to_hl_from_rst(gb, 0x571b);

find_track:
  CYC(0x571b, 0x571c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x571c, 0x571d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x571d, 0x571f);
    goto no_track_found;
  }
  CYC(0x571d, 0x571f);
  CYC(0x571f, 0x5720); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(0x5720, 0x5722);
    goto track_found;
  }
  CYC(0x5720, 0x5722);
  CYC(0x5722, 0x5724); A = 0x04;
  CYC(0x5724, 0x5725); minecart_add_a_to_hl_from_rst(gb, 0x5725);
  CYC(0x5725, 0x5727);
  goto find_track;

track_found:
  CYC(0x5727, 0x5728); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5728, 0x5729); alu_add(gb, C);
  CYC(0x5729, 0x572a); C = A;
  CYC(0x572a, 0x572c); H8(hFF8B) = A;
  CYC(0x572c, 0x572e); B = 0xce;
  CYC(0x572e, 0x572f); A = mem_rd(gb, BC);
  CYC(0x572f, 0x5731); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x5731, 0x5732); ret_effect(gb); return;
  }
  CYC(0x5731, 0x5732);
  CYC(0x5732, 0x5734); B = 0xcf;
  CYC(0x5734, 0x5735); A = mem_rd(gb, BC);
  CYC(0x5735, 0x5737); alu_cp(gb, 0x5f);
  if (F & FZ) {
    CYCT(0x5737, 0x5739);
    goto stop_minecart;
  }
  CYC(0x5737, 0x5739);
  CYC(0x5739, 0x573a); C = A;
  CYC(0x573a, 0x573c); B = 0x03;

check_linked_track:
  CYC(0x573c, 0x573d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x573d, 0x573e); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x573e, 0x5740);
    goto update_direction;
  }
  CYC(0x573e, 0x5740);
  CYC(0x5740, 0x5741); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5741, 0x5743);
    goto check_linked_track;
  }
  CYC(0x5741, 0x5743);
  CYC(0x5743, 0x5745);
  goto no_track_found;

stop_minecart:
  CYC(0x5745, 0x5746); alu_scf(gb);
  CYC(0x5746, 0x5747); ret_effect(gb);
  return;

update_direction:
  CYC(0x5747, 0x5748); A = E;
  CYC(0x5748, 0x574a); alu_sub(gb, 0x59);
  CYC(0x574a, 0x574c); alu_cp(gb, 0x06);
  if (F & FC) {
    CYCT(0x574c, 0x574e);
    goto dispatch_track;
  }
  CYC(0x574c, 0x574e);

no_track_found:
  CYC(0x574e, 0x5750); A = 0x06;

dispatch_track:
  CYC(0x5750, 0x5752); E = 0x08;
  CYC(0x5752, 0x5753); push_effect(gb, 0x5753);
  switch (minecart_jump_table(gb)) {
    case 0x5761:
      CYC(0x5761, 0x5762); A = mem_rd(gb, DE);
      CYC(0x5762, 0x5764); alu_xor(gb, 0x01);
      CYC(0x5764, 0x5765); mem_wr(gb, DE, A);
      CYC(0x5765, 0x5766); ret_effect(gb);
      return;
    case 0x5766:
      CYC(0x5766, 0x5767); A = mem_rd(gb, DE);
      CYC(0x5767, 0x5769); alu_xor(gb, 0x03);
      CYC(0x5769, 0x576a); mem_wr(gb, DE, A);
      CYC(0x576a, 0x576b); ret_effect(gb);
      return;
    case 0x576b:
      CYC(0x576b, 0x576c); A = mem_rd(gb, DE);
      CYC(0x576c, 0x576e); alu_and(gb, 0x02);
      CYC(0x576e, 0x5770); alu_or(gb, 0x01);
      CYC(0x5770, 0x5771); mem_wr(gb, DE, A);
      CYC(0x5771, 0x5772); ret_effect(gb);
      return;
    case 0x5772:
      CYC(0x5772, 0x5773); A = mem_rd(gb, DE);
      CYC(0x5773, 0x5775); alu_and(gb, 0x02);
      CYC(0x5775, 0x5776); mem_wr(gb, DE, A);
      CYC(0x5776, 0x5777); ret_effect(gb);
      return;
    case 0x5777:
      CYC(0x5777, 0x577a); push_effect(gb, 0x577a);
      minecart_check_door(gb, gb->sp);
      if (!(F & FC)) {
        CYCT(0x577a, 0x577c);
        goto reverse_direction;
      }
      CYC(0x577a, 0x577c);
      CYC(0x577c, 0x577d); alu_xor(gb, A);
      CYC(0x577d, 0x577e); ret_effect(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }

reverse_direction:
  CYC(0x577e, 0x577f); A = mem_rd(gb, DE);
  CYC(0x577f, 0x5781); alu_xor(gb, 0x02);
  CYC(0x5781, 0x5782); mem_wr(gb, DE, A);
  CYC(0x5782, 0x5783); ret_effect(gb);
}

void minecartCreateCollisionItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57dd, 0x57df); E = 0x36;
  CYC(0x57df, 0x57e0); A = mem_rd(gb, DE);
  CYC(0x57e0, 0x57e1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x57e1, 0x57e2); ret_effect(gb); return;
  }
  CYC(0x57e1, 0x57e2);
  CALL_C(0x57e2, getFreeItemSlot_hook, 0x2cf9, 0x57e5);
  if (!(F & FZ)) {
    CYCT(0x57e5, 0x57e6); ret_effect(gb); return;
  }
  CYC(0x57e5, 0x57e6);
  CYC(0x57e6, 0x57e8); E = 0x36;
  CYC(0x57e8, 0x57ea); A = 0x01;
  CYC(0x57ea, 0x57eb); mem_wr(gb, DE, A);
  CYC(0x57eb, 0x57ec); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57ec, 0x57ee); mem_wr(gb, HL, 0x1d);
  CYC(0x57ee, 0x57ef); ret_effect(gb);
}
