#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SUBROSIAN_SMITHY), bank 0x15.

void s_subrosianSmith_takeHardOre_hook(GB *gb) {
  BASE(subrosianSmith_takeHardOre);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x52; // TREASURE_HARD_ORE
  CALL_C(b_+2, loseTreasure_hook, SYM(loseTreasure), b_+5);
  TAIL_S(subrosianSmith_giveUpgradedShield);
}

// The next shield level (at most 2) as a treasure at Link's position.
void s_subrosianSmith_giveUpgradedShield_hook(GB *gb) {
  BASE(subrosianSmith_giveUpgradedShield);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01; // TREASURE_SHIELD
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto have_shield; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
have_shield:
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+10, b_+12); goto spawn; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x02;
spawn:
  CYC(b_+14, b_+15); C = A;
  CALL_C(b_+15, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+18);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x01); // TREASURE_SHIELD
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+26); mem_wr(gb, HL, C);
  CYC(b_+26, b_+27); push_effect(gb, DE);
  CYC(b_+27, b_+30); SET_DE(w1Link_yh);
  CALL_C(b_+30, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+33);
  CYC(b_+33, b_+34); SET_DE(pop_effect(gb));
  RET(b_+34); return;
}
