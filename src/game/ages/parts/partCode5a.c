#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode5a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode5a), (from), (to), true)

// PART_TRIFORCE_STONE: stone blocking path to Nayru at the start of the game
void partCode5a_hook(GB *gb) {
  BASE(partCode5a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(b_+12, b_+15); partDelete_hook(gb); return; } // jp z
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x40);
  CYC(b_+17, b_+19); A = 0x28;
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_7f52; } // jr nz
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x48;

L_7f52:
  CYC(b_+23, b_+25); E = 0xcd; // Part.xh
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+29);
  CYC(b_+29, b_+31); H = 0xcf; // >wRoomLayout
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x00);
  CYC(b_+33, b_+35); A = 0x98; // PALH_98
  CALL_C(b_+35, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+38);
  CYC(b_+38, SYM(func_11_7f64)); objectSetVisible83_hook(gb); return; // jp
}
