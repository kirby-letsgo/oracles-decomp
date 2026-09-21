#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_SOLDIER), bank 0x15.

void soldierSetSimulatedInputToEscortLink_hook(GB *gb) {
  BASE(soldierSetSimulatedInputToEscortLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+1, b_+3); goto exit_palace; } // jr nz
  CYC(b_+1, b_+3);
  CYC(b_+3, b_+6); A = W8(w1Link_xh);
  CYC(b_+6, b_+8); B = 0x60;
  CYC(b_+8, b_+10); alu_sub(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto have_x_delta; } // jr nc
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_cpl(gb);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
  CYC(b_+14, b_+16); B = 0x50;

have_x_delta:
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+18); push_effect(gb, DE);
  CYC(b_+18, b_+21); SET_HL((SYM(shootingGalleryScript_humanNpc_gameDone__checkScoreForFluteGame_b15) + 6)); // agesInteractionsBank09.linkEnterPalaceSimulatedInput
  CYC(b_+21, b_+23); A = 0x09; // :agesInteractionsBank09.linkEnterPalaceSimulatedInput
  CALL_C(b_+23, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+26);
  CYC(b_+26, b_+27); SET_DE(pop_effect(gb));
  CYC(b_+27, b_+28); A = C;
  CYC(b_+28, b_+29); alu_rra(gb);
  CYC(b_+29, b_+30); alu_add(gb, C);
  CYC(b_+30, b_+33); mem_wr(gb, wSimulatedInputCounter, A);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+37); mem_wr(gb, wSimulatedInputValue, A);
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+41); mem_wr(gb, wDisabledObjects, A);
  RET(b_+41); return;

exit_palace:
  CYC(b_+42, b_+43); push_effect(gb, DE);
  CYC(b_+43, b_+46); SET_HL((SYM(shootingGalleryScript_humanNpc_gameDone__checkScoreForFluteGame_b15) + 11)); // agesInteractionsBank09.linkExitPalaceSimulatedInput
  CYC(b_+46, b_+48); A = 0x09; // :agesInteractionsBank09.linkExitPalaceSimulatedInput
  CALL_C(b_+48, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+51);
  CYC(b_+51, b_+52); SET_DE(pop_effect(gb));
  RET(b_+52); return;
}

void soldierGiveMysterySeeds_hook(GB *gb) {
  BASE(soldierGiveMysterySeeds);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CYC(b_+2, b_+4); C = 0x00;
  CYC(b_+4, b_+7); TAIL(giveTreasure); // jp
}

void soldierUpdateMinimap_hook(GB *gb) {
  BASE(soldierUpdateMinimap);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(ghostVeranApplySpeedUntilVar38Zero) + 10)); // bank1.checkUpdateDungeonMinimap
  CYC(b_+3, b_+5); E = 0x01; // target bank
  CYC(b_+5, b_+8); interBankCall_hook(gb); return; // jp
}

static void soldier15_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void soldierGetRandomVar32Val_hook(GB *gb) {
  BASE(soldierGetRandomVar32Val);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+18); // @data
  CYC(b_+8, b_+9); soldier15_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); A = 0x59;
  CYC(b_+15, b_+16); E = alu_inc8(gb, E);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
}

void soldierSetTextToShow_hook(GB *gb) {
  BASE(soldierSetTextToShow);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+16); // @soldierTextIndices
  CYC(b_+6, b_+7); soldier15_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x59; // >TX_5900
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}
