#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  CYC(0x7b21, 0x7b24); A = mem_rd(gb, wNumSlates);
  CYC(0x7b24, 0x7b25); alu_or(gb, A);
  CYC(0x7b25, 0x7b27); B = 0x01;
  if (!(F & FZ)) { CYCT(0x7b27, 0x7b29); goto store; }
  CYC(0x7b27, 0x7b29);
  CYC(0x7b29, 0x7b2a); B = alu_dec8(gb, B);
store:
  CYC(0x7b2a, 0x7b2b); A = B;
  CYC(0x7b2b, 0x7b2e); mem_wr(gb, 0xcfc1, A);
  CYC(0x7b2e, 0x7b2f); ret_effect(gb);
}

void slateSlot_placeSlate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b2f, 0x7b31); A = 0x70;
  CALL_C(0x7b31, playSound_b00_hook, 0x0c98, 0x7b34);
  CALL_C(0x7b34, objectGetTileAtPosition_hook, 0x1444, 0x7b37);
  CYC(0x7b37, 0x7b38); C = L;
  CYC(0x7b38, 0x7b3a); E = 0x42;
  CYC(0x7b3a, 0x7b3b); A = mem_rd(gb, DE);
  CYC(0x7b3b, 0x7b3c); B = A;
  CYC(0x7b3c, 0x7b3e); A = 0xd4;
  CYC(0x7b3e, 0x7b3f); alu_add(gb, B);
  CALL_C(0x7b3f, setTile_hook, 0x3a9c, 0x7b42);
  CALL_C(0x7b42, getThisRoomFlags_hook, 0x197d, 0x7b45);
  CYC(0x7b45, 0x7b47); E = 0x42;
  CYC(0x7b47, 0x7b48); A = mem_rd(gb, DE);
  CYC(0x7b48, 0x7b4b); SET_BC(bitTable);
  CYC(0x7b4b, 0x7b4c); alu_add(gb, C);
  CYC(0x7b4c, 0x7b4d); C = A;
  CYC(0x7b4d, 0x7b4e); A = mem_rd(gb, BC);
  CYC(0x7b4e, 0x7b4f); alu_or(gb, mem_rd(gb, HL));
  CYC(0x7b4f, 0x7b50); mem_wr(gb, HL, A);
  CYC(0x7b50, 0x7b53); SET_HL(wNumSlates);
  CYC(0x7b53, 0x7b54); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7b54, 0x7b56); E = 0x42;
  CYC(0x7b56, 0x7b57); A = mem_rd(gb, DE);
  CYC(0x7b57, 0x7b5a); SET_HL(0x7b6b);
  CYC(0x7b5a, 0x7b5b); slateSlot_add_double_index(gb, 0x7b5b);
  CYC(0x7b5b, 0x7b5c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7b5c, 0x7b5d); C = A;
  CYC(0x7b5d, 0x7b5f); A = 0x09;
  CYC(0x7b5f, 0x7b60); push_effect(gb, HL);
  CALL_C(0x7b60, setTile_hook, 0x3a9c, 0x7b63);
  CYC(0x7b63, 0x7b64); SET_HL(pop_effect(gb));
  CYC(0x7b64, 0x7b65); A = mem_rd(gb, HL);
  CYC(0x7b65, 0x7b66); C = A;
  CYC(0x7b66, 0x7b68); A = 0x09;
  CYC(0x7b68, 0x7b6b); setTile_hook(gb);
}
