#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

static void add_double_index_to_hl(GB *gb) {
  CYC(0x6b33, 0x6b34); push_effect(gb, 0x6b34);
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

void objectLoadMovementScript_body_hook(GB *gb) {
  CYC(0x6b2d, 0x6b2f); A = H8(hActiveObjectType);
  CYC(0x6b2f, 0x6b31); alu_add(gb, 0x02);
  CYC(0x6b31, 0x6b32); E = A;
  CYC(0x6b32, 0x6b33); A = mem_rd(gb, DE);
  add_double_index_to_hl(gb);
  CYC(0x6b34, 0x6b35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6b35, 0x6b36); H = mem_rd(gb, HL);
  CYC(0x6b36, 0x6b37); L = A;
  CYC(0x6b37, 0x6b38); A = E;
  CYC(0x6b38, 0x6b3a); alu_add(gb, 0x0e);
  CYC(0x6b3a, 0x6b3b); E = A;
  CYC(0x6b3b, 0x6b3c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6b3c, 0x6b3d); mem_wr(gb, DE, A);
  CYC(0x6b3d, 0x6b3e); A = E;
  CYC(0x6b3e, 0x6b40); alu_add(gb, 0xf8);
  CYC(0x6b40, 0x6b41); E = A;
  CYC(0x6b41, 0x6b42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6b42, 0x6b43); mem_wr(gb, DE, A);
  CYC(0x6b43, 0x6b44); A = E;
  CYC(0x6b44, 0x6b46); alu_add(gb, 0x28);
  CYC(0x6b46, 0x6b47); E = A;
  CYC(0x6b47, 0x6b48); A = L;
  CYC(0x6b48, 0x6b49); mem_wr(gb, DE, A);
  CYC(0x6b49, 0x6b4a); E = alu_inc8(gb, E);
  CYC(0x6b4a, 0x6b4b); A = H;
  CYC(0x6b4b, 0x6b4c); mem_wr(gb, DE, A);
  objectRunMovementScript_body(gb);
}
