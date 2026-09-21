#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void slateSlot_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void slateSlot_7b21_hook(GB *gb) {
  BASE(slateSlot_7b21);
  CYC(b_+0, b_+3); A = mem_rd(gb, wNumSlates);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); B = 0x01;
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto store; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);
store:
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void slateSlot_placeSlate_hook(GB *gb) {
  BASE(slateSlot_placeSlate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x70;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+9); C = L;
  CYC(b_+9, b_+11); E = 0x42;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15); A = 0xd4;
  CYC(b_+15, b_+16); alu_add(gb, B);
  CALL_C(b_+16, setTile_hook, SYM(setTile), b_+19);
  CALL_C(b_+19, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+22);
  CYC(b_+22, b_+24); E = 0x42;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+28); SET_BC(bitTable);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+31); A = mem_rd(gb, BC);
  CYC(b_+31, b_+32); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+36); SET_HL(wNumSlates);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+39); E = 0x42;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+43); SET_HL(b_+60);
  CYC(b_+43, b_+44); slateSlot_add_double_index(gb, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); C = A;
  CYC(b_+46, b_+48); A = 0x09;
  CYC(b_+48, b_+49); push_effect(gb, HL);
  CALL_C(b_+49, setTile_hook, SYM(setTile), b_+52);
  CYC(b_+52, b_+53); SET_HL(pop_effect(gb));
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+57); A = 0x09;
  CYC(b_+57, b_+60); TAIL(setTile);
}
