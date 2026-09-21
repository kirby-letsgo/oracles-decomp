#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/roomSpecificTileChanges.s, bank $04: per-room layout patches
// applied after the room loads.

// Horon Village: on a GBC the clock shop sign shows.
void s_tileReplacement_group0Mapc5_hook(GB *gb) {
  BASE(tileReplacement_group0Mapc5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hGameboyType);
  CYC(b_+2, b_+3); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout + 0x14);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xea);
  RET(b_+9); return;
}

// Open Maku tree gates once room flag bit 7 is set.
void s_tileReplacement_group0Mapd9_hook(GB *gb) {
  BASE(tileReplacement_group0Mapd9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x14);
  CYC(b_+9, b_+11); A = 0xbf;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x24;
  CYC(b_+15, b_+17); A = 0xa9;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  RET(b_+20); return;
}

// Torch puzzle room: a lit torch once room flag bit 6 is set, otherwise water over the
// positions listed in the table (pairs of position, tile; two tiles per row).
void s_tileReplacement_group0Mape4_hook(GB *gb) {
  BASE(tileReplacement_group0Mape4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+5, b_+7); goto water; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wRoomLayout + 0x77);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xa1); // TILEINDEX_OVERWORLD_LIT_TORCH
  RET(b_+12); return;

water:
  CYC(b_+13, b_+16); SET_HL(b_+28); // @table_group0Mape4
  CYC(b_+16, b_+18); D = 0xcf;
  for (;;) {
    CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+19, b_+20); alu_or(gb, A);
    if (F & FZ) { RET_TAKEN(b_+20); return; }
    CYC(b_+20, b_+21);
    CYC(b_+21, b_+22); E = A;
    CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+23, b_+24); mem_wr(gb, DE, A);
    CYC(b_+24, b_+25); E = alu_inc8(gb, E);
    CYC(b_+25, b_+26); mem_wr(gb, DE, A);
    CYCT(b_+26, b_+28);
  }
}
