#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/roomGfxChanges.s, bank $04: per-room graphics and collision
// patches applied after the room loads.

// Before the intro is done, load Din's troupe tiles (VRAM and collisions) from the table after
// the routine; the WRAM and ROM banks are pushed for the loader to restore.
void s_roomTileChangesAfterLoad0e_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = mem_rd(gb, IO_SVBK);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+11); A = mem_rd(gb, hRomBank);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); push_effect(gb, BC);
  CYC(b_+13, b_+16); SET_DE(b_+19); // @vramTilesAndCollisions
  CYC(b_+16, b_+19);
  TAIL_SG(loadDinsTroupeTileChanges);
}

void s_roomTileChangesAfterLoad0f_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDiggingUpEnemiesForbidden, A);
  RET(b_+5); return;
}
