#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank1.s, the Seasons-only pieces.

static void bank1_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// After a warp, the season of the new room pack: always spring once the flag is set, the
// companion region's for packs $f0+, otherwise the pack's fixed season from roomPackSeasonTable.
void s_checkRoomPackAfterWarp_body_hook(GB *gb) {
  BASE(checkRoomPackAfterWarp_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wRoomPack);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL_SG(determineSeasonForRoomPack); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+13, b_+16); TAIL_SG(determineCompanionRegionSeason); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); SET_HL(SYM(roomPackSeasonTable));
  CYC(b_+21, b_+22); bank1_add_a_to_hl(gb, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+26); mem_wr(gb, wRoomStateModifier, A);
  RET(b_+26); return;
}
