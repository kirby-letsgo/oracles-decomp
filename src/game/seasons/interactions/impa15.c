#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_IMPA), bank 0x15.

// wTmpcfc0 = 0 once the fourth essence is obtained, else 1.
void s_impa_checkIf4thEssenceGotten_hook(GB *gb) {
  BASE(impa_checkIf4thEssenceGotten);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  CYC(b_+5, b_+7); alu_and(gb, 0x08);
  CYC(b_+7, b_+9); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto store; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); B = alu_inc8(gb, B);
store:
  CYC(b_+12, b_+15); SET_HL(wTmpcfc0);
  CYC(b_+15, b_+16); mem_wr(gb, HL, B);
  RET(b_+16); return;
}
