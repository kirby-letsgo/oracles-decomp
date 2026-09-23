#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_BIGGORON), bank 0x15.

void s_biggoron_loadAnimationData_hook(GB *gb) {
  BASE(biggoron_loadAnimationData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(loadAnimationData);
}

// var3f = 1 once the trade item is the soup or later (index 5+).
void s_biggoron_checkSoupGiven_hook(GB *gb) {
  BASE(biggoron_checkSoupGiven);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x41; // TREASURE_TRADEITEM
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0x7f; // Interaction.var3f
  if (!(F & FC)) { CYCT(b_+8, b_+10); goto not_given; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x05);
  if (F & FC) { CYCT(b_+12, b_+14); goto not_given; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x01);
  RET(b_+16); return;
not_given:
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  RET(b_+19); return;
}

void s_biggoron_createSparkleAtLink_hook(GB *gb) {
  BASE(biggoron_createSparkleAtLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(b_+6, b_+7); push_effect(gb, DE);
  CYC(b_+7, b_+10); SET_DE(w1Link_yh);
  CALL_C(b_+10, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+13);
  CYC(b_+13, b_+14); SET_DE(pop_effect(gb));
  RET(b_+14); return;
}
