#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_AMBI), bank 0x15.

void ambiFlickerVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5cb1, 0x5cb3); B = 0x01;
  CYC(0x5cb3, 0x5cb6); objectFlickerVisibility_hook(gb); return; // jp
}

void ambiDecVar3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5cb6, 0x5cb7); H = D;
  CYC(0x5cb7, 0x5cb9); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5cb9, 0x5cba); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5cba, 0x5cbd); writeFlagsTocddb_hook(gb); return; // jp
}

// Ambi rises by 4 pixels per frame until z-position = -$40.
void ambiRiseUntilOffScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5cbd, 0x5cbf); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5cbf, 0x5cc0); A = mem_rd(gb, DE);
  CYC(0x5cc0, 0x5cc2); alu_sub(gb, 0x04);
  CYC(0x5cc2, 0x5cc3); mem_wr(gb, DE, A);
  CYC(0x5cc3, 0x5cc5); alu_cp(gb, 0xc0);
  CYC(0x5cc5, 0x5cc8); writeFlagsTocddb_hook(gb); return; // jp
}
