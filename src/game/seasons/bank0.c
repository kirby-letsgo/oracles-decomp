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

// cpRupeeValue+3
static void bank0_d_cpRupeeValue_3(GB *gb) {
  BASE(cpRupeeValue);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+3, s_getRupeeValue, SYM(getRupeeValue), b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); H = mem_rd(gb, HL);
  CYC(b_+8, b_+9); L = A;
  CALL_C(b_+9, s_compareHlToBc, SYM(compareHlToBc), b_+12);
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_174a; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  RET(b_+16); return;
L_174a:
  CYC(b_+17, b_+18); alu_xor(gb, A);
  RET(b_+18); return;
}

// Same as below but for ore chunks.
void s_cpOreChunkValue_hook(GB *gb) {
  BASE(cpOreChunkValue);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wNumOreChunks);
  CYC(b_+3, b_+5);
  bank0_d_cpRupeeValue_3(gb); return;
}

// removeRupeeValue+3
static void bank0_d_removeRupeeValue_3(GB *gb) {
  BASE(removeRupeeValue);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+3, s_getRupeeValue, SYM(getRupeeValue), b_+6);
  CYC(b_+6, b_+9);
  TAIL(subDecimalFromHlRef);
}

void s_removeOreChunkValue_hook(GB *gb) {
  BASE(removeOreChunkValue);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wNumOreChunks);
  CYC(b_+3, b_+5);
  bank0_d_removeRupeeValue_3(gb); return;
}

void s_seasonsFunc_331b_hook(GB *gb) {
  BASE(seasonsFunc_331b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x0f;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_seasonsFunc_0f_6f75_hook, SYM(seasonsFunc_0f_6f75), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_seasonsFunc_332f_hook(GB *gb) {
  BASE(seasonsFunc_332f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x0f;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_seasonsFunc_0f_704d_hook, SYM(seasonsFunc_0f_704d), b_+13);
  CALL_C(b_+13, s_seasonsFunc_0f_7182_hook, SYM(seasonsFunc_0f_7182), b_+16);
  CYC(b_+16, b_+17); SET_AF(pop_effect(gb));
  CYC(b_+17, b_+19); mem_wr(gb, hRomBank, A);
  CYC(b_+19, b_+22); mem_wr(gb, 0x2222, A);
  RET(b_+22); return;
}

void s_flameOfDestructionsCutsceneCaller_hook(GB *gb) {
  BASE(flameOfDestructionsCutsceneCaller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_flameOfDestructionCutsceneBody_hook, SYM(flameOfDestructionCutsceneBody), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_zeldaAndVillagersCutsceneCaller_hook(GB *gb) {
  BASE(zeldaAndVillagersCutsceneCaller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_zeldaAndVillagersCutsceneBody_hook, SYM(zeldaAndVillagersCutsceneBody), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_zeldaKidnappedCutsceneCaller_hook(GB *gb) {
  BASE(zeldaKidnappedCutsceneCaller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_zeldaKidnappedCutsceneBody_hook, SYM(zeldaKidnappedCutsceneBody), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_seasonsFunc_34a0_hook(GB *gb) {
  BASE(seasonsFunc_34a0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x05;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_updateSpecialObjects, SYM(updateSpecialObjects), b_+13);
  CYC(b_+13, b_+15); A = 0x07;
  CYC(b_+15, b_+17); mem_wr(gb, hRomBank, A);
  CYC(b_+17, b_+20); mem_wr(gb, 0x2222, A);
  CALL_C(b_+20, s_updateItems, SYM(updateItems), b_+23);
  CYC(b_+23, b_+25); A = 0x00;
  CYC(b_+25, b_+27); mem_wr(gb, hRomBank, A);
  CYC(b_+27, b_+30); mem_wr(gb, 0x2222, A);
  CALL_C(b_+30, s_updateEnemies, SYM(updateEnemies), b_+33);
  CYC(b_+33, b_+35); A = 0x10;
  CYC(b_+35, b_+37); mem_wr(gb, hRomBank, A);
  CYC(b_+37, b_+40); mem_wr(gb, 0x2222, A);
  CALL_C(b_+40, s_updateParts, SYM(updateParts), b_+43);
  CYC(b_+43, b_+45); A = 0x00;
  CYC(b_+45, b_+47); mem_wr(gb, hRomBank, A);
  CYC(b_+47, b_+50); mem_wr(gb, 0x2222, A);
  CALL_C(b_+50, s_updateInteractions, SYM(updateInteractions), b_+53);
  CYC(b_+53, b_+55); A = 0x0f;
  CYC(b_+55, b_+57); mem_wr(gb, hRomBank, A);
  CYC(b_+57, b_+60); mem_wr(gb, 0x2222, A);
  CALL_C(b_+60, s_seasonsFunc_0f_7159_hook, SYM(seasonsFunc_0f_7159), b_+63);
  CYC(b_+63, b_+65); A = 0x06;
  CYC(b_+65, b_+67); mem_wr(gb, hRomBank, A);
  CYC(b_+67, b_+70); mem_wr(gb, 0x2222, A);
  CYC(b_+70, b_+73); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+73, b_+74); alu_rlca(gb);
  if (F & FC) CALL_C_CC(b_+74, s_updateGrabbedObjectPosition, SYM(updateGrabbedObjectPosition), b_+77);
  else CYC(b_+74, b_+77);
  CALL_C(b_+77, s_loadLinkAndCompanionAnimationFrame, SYM(loadLinkAndCompanionAnimationFrame), b_+80);
  CYC(b_+80, b_+82); A = 0x07;
  CYC(b_+82, b_+84); mem_wr(gb, hRomBank, A);
  CYC(b_+84, b_+87); mem_wr(gb, 0x2222, A);
  CALL_C(b_+87, s_updateItemsPost, SYM(updateItemsPost), b_+90);
  CYC(b_+90, b_+92); A = 0x0f;
  CYC(b_+92, b_+94); mem_wr(gb, hRomBank, A);
  CYC(b_+94, b_+97); mem_wr(gb, 0x2222, A);
  CALL_C(b_+97, s_seasonsFunc_0f_7182_hook, SYM(seasonsFunc_0f_7182), b_+100);
  CYC(b_+100, b_+102); A = 0x04;
  CYC(b_+102, b_+104); mem_wr(gb, hRomBank, A);
  CYC(b_+104, b_+107); mem_wr(gb, 0x2222, A);
  CALL_C(b_+107, s_updateChangedTileQueue, SYM(updateChangedTileQueue), b_+110);
  CYC(b_+110, b_+111); alu_xor(gb, A);
  CYC(b_+111, b_+114); mem_wr(gb, wc4b6, A);
  CYC(b_+114, b_+115); SET_AF(pop_effect(gb));
  CYC(b_+115, b_+117); mem_wr(gb, hRomBank, A);
  CYC(b_+117, b_+120); mem_wr(gb, 0x2222, A);
  RET(b_+120); return;
}

void s_seasonsFunc_35cc_hook(GB *gb) {
  BASE(seasonsFunc_35cc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+5); A = mem_rd(gb, hRomBank);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); push_effect(gb, BC);
  CYC(b_+7, b_+9); A = 0x02;
  CYC(b_+9, b_+11); mem_wr(gb, IO_SVBK, A);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+15); mem_wr(gb, hRomBank, A);
  CYC(b_+15, b_+18); mem_wr(gb, 0x2222, A);
  CALL_C(b_+18, s_paletteThread_calculateFadingPalettes, SYM(paletteThread_calculateFadingPalettes), b_+21);
  CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+25); mem_wr(gb, hRomBank, A);
  CYC(b_+25, b_+28); mem_wr(gb, 0x2222, A);
  CYC(b_+28, b_+29); A = C;
  CYC(b_+29, b_+31); mem_wr(gb, IO_SVBK, A);
  RET(b_+31); return;
}

// paletteFadeHandler08@seasonsFunc_01_5816
static void bank0_d_seasonsFunc_01_5816(GB *gb) {
  BASE(paletteFadeHandler08);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+15); mem_wr(gb, hFF91, A);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+18); alu_sub(gb, B);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+22); mem_wr(gb, hFF90, A);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_rrca(gb);
  if (F & FC) { CYCT(b_+24, b_+27); TAIL(paletteThread_mixBG234Palettes); }
  CYC(b_+24, b_+27);
  CALL_C(b_+27, s_paletteThread_mixBG567Palettes, SYM(paletteThread_mixBG567Palettes), b_+30);
  CYC(b_+30, b_+32); A = mem_rd(gb, hDirtyBgPalettes);
  CYC(b_+32, b_+34); alu_or(gb, 0xfc);
  CYC(b_+34, b_+36); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+36, b_+38); A = 0xfc;
  CYC(b_+38, b_+40); mem_wr(gb, hBgPaletteSources, A);
  RET(b_+40); return;
}

void s_func_35ec_hook(GB *gb) {
  BASE(func_35ec);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x01;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_L(b_+10, bank0_d_seasonsFunc_01_5816, b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_checkRoomPackAfterWarp_hook(GB *gb) {
  BASE(checkRoomPackAfterWarp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x01;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_checkRoomPackAfterWarp_body_hook, SYM(checkRoomPackAfterWarp_body), b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+16); mem_wr(gb, hRomBank, A);
  CYC(b_+16, b_+19); mem_wr(gb, 0x2222, A);
  RET(b_+19); return;
}

void s_createSokraSnore_hook(GB *gb) {
  BASE(createSokraSnore);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0xfa;
  CYC(b_+8, b_+10); C = 0xfc;
  CYC(b_+10, b_+13);
  TAIL(objectCreateFloatingSnore);
}

void s_checkGotMakuSeedDidNotSeeZeldaKidnapped_hook(GB *gb) {
  BASE(checkGotMakuSeedDidNotSeeZeldaKidnapped);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x0a;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_checkGotMakuSeedDidNotSeeZeldaKidnapped_body_hook, SYM(checkGotMakuSeedDidNotSeeZeldaKidnapped_body), b_+13);
  CYC(b_+13, b_+14); push_effect(gb, AF);
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  CYC(b_+15, b_+16); SET_AF(pop_effect(gb));
  CYC(b_+16, b_+18); mem_wr(gb, hRomBank, A);
  CYC(b_+18, b_+21); mem_wr(gb, 0x2222, A);
  RET(b_+21); return;
}

// When Maku tree speaks from other screens?
void s_setMakuTreeStageAndMapText_hook(GB *gb) {
  BASE(setMakuTreeStageAndMapText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x09;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_makuTree_setAppropriateStage_hook, SYM(makuTree_setAppropriateStage), b_+13);
  CYC(b_+13, b_+15); A = 0x15;
  CYC(b_+15, b_+17); mem_wr(gb, hRomBank, A);
  CYC(b_+17, b_+20); mem_wr(gb, 0x2222, A);
  CALL_C(b_+20, s_makuTree_setMapTextBasedOnStage_hook, SYM(makuTree_setMapTextBasedOnStage), b_+23);
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+26); mem_wr(gb, hRomBank, A);
  CYC(b_+26, b_+29); mem_wr(gb, 0x2222, A);
  RET(b_+29); return;
}

void s_setUpCharactersAfterMoblinKeepDestroyed_hook(GB *gb) {
  BASE(setUpCharactersAfterMoblinKeepDestroyed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x0a;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CALL_C(b_+10, s_moblinKeepScene_setLinkDirectionAndPositionAfterDestroyed_hook, SYM(moblinKeepScene_setLinkDirectionAndPositionAfterDestroyed), b_+13);
  CYC(b_+13, b_+15); A = 0x01;
  CALL_C(b_+15, s_moblinKeepScene_spawnKingMoblin_hook, SYM(moblinKeepScene_spawnKingMoblin), b_+18);
  CALL_C(b_+18, s_moblinKeepScene_spawn2MoblinsAfterKeepDestroyed_hook, SYM(moblinKeepScene_spawn2MoblinsAfterKeepDestroyed), b_+21);
  CYC(b_+21, b_+22); SET_AF(pop_effect(gb));
  CYC(b_+22, b_+24); mem_wr(gb, hRomBank, A);
  CYC(b_+24, b_+27); mem_wr(gb, 0x2222, A);
  RET(b_+27); return;
}

void s_getLinkedHerosCaveSideEntranceRoom_hook(GB *gb) {
  BASE(getLinkedHerosCaveSideEntranceRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+7); mem_wr(gb, hRomBank, A);
  CYC(b_+7, b_+10); mem_wr(gb, 0x2222, A);
  CYC(b_+10, b_+13); SET_HL(SYM(dimitriStateD));
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+17); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+17, b_+18); SET_AF(pop_effect(gb));
  CYC(b_+18, b_+20); mem_wr(gb, hRomBank, A);
  CYC(b_+20, b_+23); mem_wr(gb, 0x2222, A);
  RET(b_+23); return;
}

