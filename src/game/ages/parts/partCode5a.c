#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_TRIFORCE_STONE: stone blocking path to Nayru at the start of the game
void partCode5a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f3b, 0x7f3d); E = 0xc4; // Part.state
  CYC(0x7f3d, 0x7f3e); A = mem_rd(gb, DE);
  CYC(0x7f3e, 0x7f3f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7f3f); return; } // ret nz
  CYC(0x7f3f, 0x7f40);
  CYC(0x7f40, 0x7f41); A = alu_inc8(gb, A);
  CYC(0x7f41, 0x7f42); mem_wr(gb, DE, A);
  CALL_C(0x7f42, getThisRoomFlags_hook, 0x197d, 0x7f45);
  CYC(0x7f45, 0x7f47); alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(0x7f47, 0x7f4a); partDelete_hook(gb); return; } // jp z
  CYC(0x7f47, 0x7f4a);
  CYC(0x7f4a, 0x7f4c); alu_and(gb, 0x40);
  CYC(0x7f4c, 0x7f4e); A = 0x28;
  if (!(F & FZ)) { CYCT(0x7f4e, 0x7f50); goto L_7f52; } // jr nz
  CYC(0x7f4e, 0x7f50);
  CYC(0x7f50, 0x7f52); A = 0x48;

L_7f52:
  CYC(0x7f52, 0x7f54); E = 0xcd; // Part.xh
  CYC(0x7f54, 0x7f55); mem_wr(gb, DE, A);
  CALL_C(0x7f55, objectMakeTileSolid_hook, 0x20b2, 0x7f58);
  CYC(0x7f58, 0x7f5a); H = 0xcf; // >wRoomLayout
  CYC(0x7f5a, 0x7f5c); mem_wr(gb, HL, 0x00);
  CYC(0x7f5c, 0x7f5e); A = 0x98; // PALH_98
  CALL_C(0x7f5e, loadPaletteHeader_hook, 0x050b, 0x7f61);
  CYC(0x7f61, 0x7f64); objectSetVisible83_hook(gb); return; // jp
}
