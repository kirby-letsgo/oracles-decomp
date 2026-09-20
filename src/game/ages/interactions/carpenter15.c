#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void carpenter_buildBridgeColumn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x759b, 0x759c); C = A;
  CYC(0x759c, 0x759e); A = 0x1d;
  CALL_C(0x759e, setTile_hook, 0x3a9c, 0x75a1);
  CYC(0x75a1, 0x75a2); A = C;
  CYC(0x75a2, 0x75a4); alu_add(gb, 0x10);
  CYC(0x75a4, 0x75a5); C = A;
  CYC(0x75a5, 0x75a7); A = 0x1e;
  CALL_C(0x75a7, setTile_hook, 0x3a9c, 0x75aa);
  CYC(0x75aa, 0x75ad); SET_HL(wTmpcfc0_carpenterSearch_cfd0);
  CYC(0x75ad, 0x75ae); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x75ae, 0x75b0); A = 0x70;
  CYC(0x75b0, 0x75b3); playSound_b00_hook(gb);
}
