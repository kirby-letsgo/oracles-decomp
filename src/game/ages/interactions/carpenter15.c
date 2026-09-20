#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(carpenter_buildBridgeColumn), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(carpenter_buildBridgeColumn), (from), (to), true)

void carpenter_buildBridgeColumn_hook(GB *gb) {
  BASE(carpenter_buildBridgeColumn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); A = 0x1d;
  CALL_C(b_+3, setTile_hook, SYM(setTile), b_+6);
  CYC(b_+6, b_+7); A = C;
  CYC(b_+7, b_+9); alu_add(gb, 0x10);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+12); A = 0x1e;
  CALL_C(b_+12, setTile_hook, SYM(setTile), b_+15);
  CYC(b_+15, b_+18); SET_HL(wTmpcfc0_carpenterSearch_cfd0);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); A = 0x70;
  CYC(b_+21, SYM(carpenter_subid00Script_body)); playSound_b00_hook(gb);
}
