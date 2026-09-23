#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_ZELDA_KIDNAPPED_ROOM), bank 0x15.

void s_zeldaKidnappedRoom_loadImpa_hook(GB *gb) {
  BASE(zeldaKidnappedRoom_loadImpa);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(zeldaKidnapped_impaData));
  CYCT(b_+3, b_+5);
  TAIL_S(zeldaKidnapped_spawnInteraction);
}

// King Moblin, Zelda and four moblins from consecutive entries.
void s_zeldaKidnappedRoom_loadZeldaAndMoblins_hook(GB *gb) {
  BASE(zeldaKidnappedRoom_loadZeldaAndMoblins);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(zeldaKidnapped_kingMoblinData));
  CALL_C(b_+3, s_zeldaKidnapped_spawnInteraction_hook, SYM(zeldaKidnapped_spawnInteraction), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(zeldaKidnapped_zeldaData));
  CALL_C(b_+9, s_zeldaKidnapped_spawnInteraction_hook, SYM(zeldaKidnapped_spawnInteraction), b_+12);
  CYC(b_+12, b_+15); SET_BC(SYM(zeldaKidnapped_moblinData));
  CALL_C(b_+15, s_zeldaKidnapped_spawnInteraction_hook, SYM(zeldaKidnapped_spawnInteraction), b_+18);
  CALL_C(b_+18, s_zeldaKidnapped_spawnInteraction_hook, SYM(zeldaKidnapped_spawnInteraction), b_+21);
  CALL_C(b_+21, s_zeldaKidnapped_spawnInteraction_hook, SYM(zeldaKidnapped_spawnInteraction), b_+24);
  TAIL_S(zeldaKidnapped_spawnInteraction);
}

// An interaction from the 5-byte entry at bc (id, subid, var03, yh, xh); bc moves past it.
void s_zeldaKidnapped_spawnInteraction_hook(GB *gb) {
  BASE(zeldaKidnapped_spawnInteraction);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, BC);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); SET_BC(BC + 1);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); SET_BC(BC + 1);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); SET_BC(BC + 1);
  CYC(b_+13, b_+15); L = 0x4b; // Interaction.yh
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+18); SET_BC(BC + 1);
  CYC(b_+18, b_+20); L = 0x4d; // Interaction.xh
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+23); SET_BC(BC + 1);
  RET(b_+23); return;
}

// The swimming challenge time (wcce2, wcce3) as two text numbers.
void s_linkedFunc_15_6430_hook(GB *gb) {
  BASE(linkedFunc_15_6430);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcce2);
  CYC(b_+3, b_+6); SET_HL(wcbaa);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+12); A = mem_rd(gb, wcce3);
  CYC(b_+12, b_+15); SET_HL(wTextNumberSubstitution);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  RET(b_+18); return;
}
