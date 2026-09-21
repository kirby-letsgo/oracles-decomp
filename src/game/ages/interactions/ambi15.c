#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_AMBI), bank 0x15.

void ambiFlickerVisibility_hook(GB *gb) {
  BASE(ambiFlickerVisibility);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); B = 0x01;
  CYC(b_+2, b_+5); TAIL(objectFlickerVisibility); // jp
}

void ambiDecVar3f_hook(GB *gb) {
  BASE(ambiDecVar3f);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); TAIL(writeFlagsTocddb); // jp
}

// Ambi rises by 4 pixels per frame until z-position = -$40.
void ambiRiseUntilOffScreen_hook(GB *gb) {
  BASE(ambiRiseUntilOffScreen);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x04);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); alu_cp(gb, 0xc0);
  CYC(b_+8, b_+11); TAIL(writeFlagsTocddb); // jp
}
