#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank0.s, the Seasons-only pieces: bank-switching trampolines and a
// small NPC helper.

// Runs the intro cutscene e (bank 3) with the ROM bank saved and restored.
void s_multiIntroCutsceneCaller_hook(GB *gb) {
  BASE(multiIntroCutsceneCaller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+10, s_multiIntroCutsceneHandler_hook, SYM(multiIntroCutsceneHandler), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, MBC_ROM_BANK, A);
  RET(b_+19); return;
}

// updateAnimations (bank 4) with the ROM bank saved and restored.
void s_updateAnimationsAfterCutscene_hook(GB *gb) {
  BASE(updateAnimationsAfterCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+10, updateAnimations_hook, SYM(updateAnimations), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, MBC_ROM_BANK, A);
  RET(b_+19); return;
}

// For the shared NPC ids ($24..$47), adds the object's var37 to b.
void s_seasonsFunc_2678_hook(GB *gb) {
  BASE(seasonsFunc_2678);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x41; // Interaction.id
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x24);
  CYC(b_+5, b_+7); alu_cp(gb, 0x24);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = 0x77; // Interaction.var37
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, B);
  CYC(b_+12, b_+13); B = A;
  RET(b_+13); return;
}

// getSunkenCityNPCVisibleSubId@main (bank 8) with the ROM bank saved and restored.
void s_getSunkenCityNPCVisibleSubId_caller_hook(GB *gb) {
  BASE(getSunkenCityNPCVisibleSubId_caller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x08;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+10, s_getSunkenCityNPCVisibleSubId__main_hook, SYM(getSunkenCityNPCVisibleSubId) + 14, b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, MBC_ROM_BANK, A);
  RET(b_+19); return;
}

// checkHoronVillageNPCShouldBeSeen_body@main (bank 8) with the bank saved; c = 1 when seen.
void s_checkIfHoronVillageNPCShouldBeSeen_hook(GB *gb) {
  BASE(checkIfHoronVillageNPCShouldBeSeen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x08;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+10, s_checkHoronVillageNPCShouldBeSeen_body__main_hook, SYM(checkHoronVillageNPCShouldBeSeen_body) + 9, b_+13);
  CYC(b_+13, b_+15); C = 0x01;
  if (F & FC) { CYCT(b_+15, b_+17); goto restore; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); C = alu_dec8(gb, C);
restore:
  CYC(b_+18, b_+19); SET_AF(pop_effect(gb));
  CYC(b_+19, b_+21); mem_wr(gb, hRomBank, A);
  CYC(b_+21, b_+24); mem_wr(gb, MBC_ROM_BANK, A);
  RET(b_+24); return;
}
