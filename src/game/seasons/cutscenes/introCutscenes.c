#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/introCutscenes.s, bank $03: the Din-dancing
// cutscene (CUTSCENE_S_DIN_DANCING) and the helpers it shares with the file's other cutscenes.
// Cutscene scratch: wTmpcfc0+$10 is the scene's stage counter (also written by the Din-dancing
// event interaction), wTmpcfc0+$12 the tile-list flags, wTmpcbb3/4/6 timers and counters.

static uint16_t intro_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void intro_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void s_incCutsceneState2_hook(GB *gb) {
  BASE(incCutsceneState2);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void s_cutsceneDinDancing_hook(GB *gb) {
  BASE(cutsceneDinDancing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_cutsceneDinDancingHandler_hook, SYM(cutsceneDinDancingHandler), b_+3);
  CYC(b_+3, b_+6); SET_HL(wCutsceneState);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16);
  TAIL(updateAllObjects);
}

void s_cutsceneDinDancingHandler_hook(GB *gb) {
  BASE(cutsceneDinDancingHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wCutsceneState);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (intro_jump_table(gb));
    if (jt_ == SYM(cutscene06Func0) && hook_is(gb, SYM(cutscene06Func0), s_cutscene06Func0_hook)) { s_cutscene06Func0_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func1) && hook_is(gb, SYM(cutscene06Func1), s_cutscene06Func1_hook)) { s_cutscene06Func1_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func2) && hook_is(gb, SYM(cutscene06Func2), s_cutscene06Func2_hook)) { s_cutscene06Func2_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func3) && hook_is(gb, SYM(cutscene06Func3), s_cutscene06Func3_hook)) { s_cutscene06Func3_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func4) && hook_is(gb, SYM(cutscene06Func4), s_cutscene06Func4_hook)) { s_cutscene06Func4_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func5) && hook_is(gb, SYM(cutscene06Func5), s_cutscene06Func5_hook)) { s_cutscene06Func5_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func6) && hook_is(gb, SYM(cutscene06Func6), s_cutscene06Func6_hook)) { s_cutscene06Func6_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func7) && hook_is(gb, SYM(cutscene06Func7), s_cutscene06Func7_hook)) { s_cutscene06Func7_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func8) && hook_is(gb, SYM(cutscene06Func8), s_cutscene06Func8_hook)) { s_cutscene06Func8_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Func9) && hook_is(gb, SYM(cutscene06Func9), s_cutscene06Func9_hook)) { s_cutscene06Func9_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funca) && hook_is(gb, SYM(cutscene06Funca), s_cutscene06Funca_hook)) { s_cutscene06Funca_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funcb) && hook_is(gb, SYM(cutscene06Funcb), s_cutscene06Funcb_hook)) { s_cutscene06Funcb_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funcc) && hook_is(gb, SYM(cutscene06Funcc), s_cutscene06Funcc_hook)) { s_cutscene06Funcc_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funcd) && hook_is(gb, SYM(cutscene06Funcd), s_cutscene06Funcd_hook)) { s_cutscene06Funcd_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funce) && hook_is(gb, SYM(cutscene06Funce), s_cutscene06Funce_hook)) { s_cutscene06Funce_hook(gb); return; }
    else if (jt_ == SYM(cutscene06Funcf) && hook_is(gb, SYM(cutscene06Funcf), s_cutscene06Funcf_hook)) { s_cutscene06Funcf_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void s_cutscene06Func0_hook(GB *gb) {
  BASE(cutscene06Func0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+5); A = 0x55; // SND_CLOSEMENU
  CALL_C(b_+5, playSound_b00_hook, SYM(playSound_b00), b_+8);
  TAIL_S(cutscene06Func1);
}

void s_cutscene06Func1_hook(GB *gb) {
  BASE(cutscene06Func1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+10, b_+13); SET_HL(wMenuDisabled);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x01);
  CYC(b_+15, b_+18); SET_HL(w1Link_visible);
  CYC(b_+18, b_+20); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+20, saveGraphicsOnEnterMenu_hook, SYM(saveGraphicsOnEnterMenu), b_+23);
  CYC(b_+23, b_+25); A = 0x0c; // GFXH_DIN_DANCING_CUTSCENE
  CALL_C(b_+25, loadGfxHeader_hook, SYM(loadGfxHeader), b_+28);
  CYC(b_+28, b_+30); A = 0x95; // PALH_SEASONS_95
  CALL_C(b_+30, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+33);
  CYC(b_+33, b_+35); A = 0x04;
  CALL_C(b_+35, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+38);
  CYC(b_+38, b_+41); SET_HL(wTmpcbb3);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x58);
  CYC(b_+43, b_+44); SET_HL(HL + 1);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x02);
  CYC(b_+46, b_+49); SET_HL(wTmpcbb6);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x28);
  CALL_C(b_+51, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+54);
  CALL_C(b_+54, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+57);
  CYC(b_+57, b_+60); SET_HL(wTmpcbb5);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x02);
  TAIL_S(seasonsFunc_03_7386);
}

// Draws the cutscene's static sprites, scrolled against wGfxRegs1.SCX.
void s_seasonsFunc_03_7386_hook(GB *gb) {
  BASE(seasonsFunc_03_7386);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+5); B = 0x00;
  CYC(b_+5, b_+8); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+8, b_+9); alu_cpl(gb);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+14); SET_HL(b_+17); // seasonsOamData_03_7397
  CYC(b_+14, b_+17);
  TAIL(addSpritesToOam_withOffset);
}

void s_cutscene06Func2_hook(GB *gb) {
  BASE(cutscene06Func2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); TAIL_S(seasonsFunc_03_7386); }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, s_seasonsFunc_03_7458_hook, SYM(seasonsFunc_03_7458), b_+10);
  CALL_C(b_+10, s_seasonsFunc_03_7386_hook, SYM(seasonsFunc_03_7386), b_+13);
  CYC(b_+13, b_+16); SET_HL(wTmpcbb3);
  CALL_C(b_+16, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+19);
  if (F & FZ) { CYCT(b_+19, b_+21); goto next_stage; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  CYC(b_+23, b_+24); L = A;
  CYC(b_+24, b_+27); SET_BC(0x00f0);
  CALL_C(b_+27, compareHlToBc_hook, SYM(compareHlToBc), b_+30);
  if (!(F & FC)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+34, b_+36); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
next_stage:
  CYC(b_+37, b_+39); A = 0x55; // SND_CLOSEMENU
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CALL_C(b_+42, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+45);
  CYC(b_+45, b_+48);
  TAIL(fastFadeoutToWhite);
}

// Every 8 frames while wTmpcbb6 is nonzero: count it down and scroll one pixel.
void s_seasonsFunc_03_7458_hook(GB *gb) {
  BASE(seasonsFunc_03_7458);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wTmpcbb6);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+16); SET_HL(wGfxRegs1_SCX);
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+17); return;
}

void s_cutscene06Func3_hook(GB *gb) {
  BASE(cutscene06Func3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); TAIL_S(seasonsFunc_03_7386); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+11, b_+14); SET_HL(w1Link_visible);
  CYC(b_+14, b_+16); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CALL_C(b_+20, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+23);
  CYC(b_+23, b_+26);
  TAIL(reloadGraphicsOnExitMenu);
}

void s_cutscene06Func4_hook(GB *gb) {
  BASE(cutscene06Func4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wTmpcbb4);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CALL_C(b_+11, s_seasonsFunc_03_74aa_hook, SYM(seasonsFunc_03_74aa), b_+14);
  CYC(b_+14, b_+17); SET_HL(w2TilesetBgPalettes + 0x10);
  CYC(b_+17, b_+20); SET_BC(0x44e8);
  CALL_C(b_+20, func_13c6_hook, SYM(func_13c6), b_+23);
  CYC(b_+23, b_+25); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+25, playSound_b00_hook, SYM(playSound_b00), b_+28);
  CYC(b_+28, b_+31);
  TAIL_S(incCutsceneState2);
}

// Counts wTmpcbb3 down; when it hits zero, advances wTmpcbb4 and spawns that entry's lightning.
void s_seasonsFunc_03_74a3_hook(GB *gb) {
  BASE(seasonsFunc_03_74a3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decCbb3_hook, SYM(decCbb3), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  TAIL_S(seasonsFunc_03_74aa);
}

// Loads entry a of seasonsTable_03_74d1 (timer, lightning subid, y, x): the timer goes to
// (hl-1), a $ff entry ends the sequence, otherwise the lightning part is spawned.
void s_seasonsFunc_03_74aa_hook(GB *gb) {
  BASE(seasonsFunc_03_74aa);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); D = H;
  CYC(b_+1, b_+2); E = L;
  CYC(b_+2, b_+3); alu_add(gb, A);
  CYC(b_+3, b_+6); SET_HL(b_+39); // seasonsTable_03_74d1
  CYC(b_+6, b_+7); intro_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); E = alu_dec8(gb, E);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  TAIL_S(seasonsFunc_03_74b6);
}

// Spawns a PART_LIGHTNING with subid, y and x from the 3 bytes at hl. Returns nz always.
void s_seasonsFunc_03_74b6_hook(GB *gb) {
  BASE(seasonsFunc_03_74b6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); D = H;
  CYC(b_+1, b_+2); E = L;
  CALL_C(b_+2, getFreePartSlot_hook, SYM(getFreePartSlot), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto done; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(b_+9, b_+11); L = 0xc2; // Part.subid
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+16); SET_DE(DE + 1);
  CYC(b_+16, b_+18); L = 0xcb; // Part.yh
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); SET_DE(DE + 1);
  CYC(b_+21, b_+22); SET_HL(HL + 1);
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
done:
  CYC(b_+24, b_+26); alu_or(gb, 0x01);
  RET(b_+26); return;
}

void s_cutscene06Func5_hook(GB *gb) {
  BASE(cutscene06Func5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x03);
  CALL_C(b_+10, s_seasonsFunc_03_7516_hook, SYM(seasonsFunc_03_7516), b_+13);
  CALL_C(b_+13, s_seasonsFunc_03_74a3_hook, SYM(seasonsFunc_03_74a3), b_+16);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); SET_HL(wTmpcbb3);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x3c);
  CYC(b_+22, b_+25);
  TAIL_S(incCutsceneState2);
}

// For each of the 4 tile-list flags set in wTmpcfc0+$12 (bits 3..0), load that list of tiles
// once.
void s_seasonsFunc_03_7516_hook(GB *gb) {
  BASE(seasonsFunc_03_7516);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcfc0 + 0x12);
  CYC(b_+3, b_+5); B = 0x03;
  for (;;) {
    CYC(b_+5, b_+6); A = mem_rd(gb, DE);
    CYC(b_+6, b_+7); C = A;
    CYC(b_+7, b_+8); A = B;
    CYC(b_+8, b_+11); SET_HL(SYM(bitTable));
    CYC(b_+11, b_+12); alu_add(gb, L);
    CYC(b_+12, b_+13); L = A;
    CYC(b_+13, b_+14); A = mem_rd(gb, HL);
    CYC(b_+14, b_+15); alu_and(gb, C);
    if (!(F & FZ)) CALL_C_CC(b_+15, s_cutsceneDinDancing_loadListOfTiles_hook, SYM(cutsceneDinDancing_loadListOfTiles), b_+18);
    else CYC(b_+15, b_+18);
    CYC(b_+18, b_+19); B = alu_dec8(gb, B);
    CYC(b_+19, b_+21); alu_bit(gb, 7, B);
    if (F & FZ) { CYCT(b_+21, b_+23); continue; }
    CYC(b_+21, b_+23);
    break;
  }
  RET(b_+23); return;
}

// Clears flag c in (de) and sets every tile of list b (@tileListTable: count, then tile
// positions in the $cfxx room layout) to the value the layout holds there.
void s_cutsceneDinDancing_loadListOfTiles_hook(GB *gb) {
  BASE(cutsceneDinDancing_loadListOfTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, C);
  CYC(b_+1, b_+2); mem_wr(gb, DE, A);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  CYC(b_+3, b_+4); push_effect(gb, DE);
  CYC(b_+4, b_+5); A = B;
  CYC(b_+5, b_+8); SET_HL(b_+33); // @tileListTable
  CYC(b_+8, b_+9); intro_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+13); B = mem_rd(gb, HL);
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  for (;;) {
    CYC(b_+14, b_+15); C = mem_rd(gb, HL);
    CYC(b_+15, b_+16); SET_HL(HL + 1);
    CYC(b_+16, b_+17); E = C;
    CYC(b_+17, b_+19); D = 0xcf;
    CYC(b_+19, b_+20); A = mem_rd(gb, DE);
    CYC(b_+20, b_+21); push_effect(gb, BC);
    CYC(b_+21, b_+22); push_effect(gb, HL);
    CALL_C(b_+22, setTile_hook, SYM(setTile), b_+25);
    CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
    CYC(b_+26, b_+27); SET_BC(pop_effect(gb));
    CYC(b_+27, b_+28); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+28, b_+30); continue; }
    CYC(b_+28, b_+30);
    break;
  }
  CYC(b_+30, b_+31); SET_DE(pop_effect(gb));
  CYC(b_+31, b_+32); SET_BC(pop_effect(gb));
  RET(b_+32); return;
}

void s_cutscene06Func6_hook(GB *gb) {
  BASE(cutscene06Func6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decCbb3_hook, SYM(decCbb3), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0c08);
  CALL_C(b_+10, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+13);
  if (F & FZ) { CYCT(b_+13, b_+15); goto show; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_BC(0x0c12);
show:
  CYC(b_+18, b_+21);
  TAIL(showText);
}

void s_cutscene06Func7_hook(GB *gb) {
  BASE(cutscene06Func7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CALL_C(b_+3, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(cutscene06Func5) - 3); // seasonsTable_03_74fa
  CYC(b_+9, b_+12);
  TAIL_S(seasonsFunc_03_74b6);
}

void s_cutscene06Func8_hook(GB *gb) {
  BASE(cutscene06Func8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x12);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_bit(gb, 4, A);
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto advance; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x4e); // INTERAC_DIN_DANCING_EVENT
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x07);
advance:
  CYC(b_+17, b_+20);
  TAIL_S(incCutsceneState2);
}

void s_cutscene06Func9_hook(GB *gb) {
  BASE(cutscene06Func9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x09;
  CALL_C(b_+2, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto stage; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x13;
stage:
  CYC(b_+9, b_+11); A = 0x05;
  TAIL_S(seasonsFunc_03_75a5);
}

// Once the stage counter reaches a: next state and show text $0c:c.
void s_seasonsFunc_03_75a5_hook(GB *gb) {
  BASE(seasonsFunc_03_75a5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+6); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+10);
  CYC(b_+10, b_+12); B = 0x0c;
  CYC(b_+12, b_+15);
  TAIL(showText);
}

void s_cutscene06Funca_hook(GB *gb) {
  BASE(cutscene06Funca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x06);
  CYC(b_+8, b_+11);
  TAIL_S(incCutsceneState2);
}

void s_cutscene06Funcb_hook(GB *gb) {
  BASE(cutscene06Funcb);
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+4); C = 0x14;
  CYC(b_+4, b_+7);
  TAIL_S(seasonsFunc_03_75a5);
}

void s_cutscene06Funcc_hook(GB *gb) {
  BASE(cutscene06Funcc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb3);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x1e);
  CYC(b_+8, b_+11);
  TAIL_S(incCutsceneState2);
}

void s_cutscene06Funcd_hook(GB *gb) {
  BASE(cutscene06Funcd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decCbb3_hook, SYM(decCbb3), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x09);
  CYC(b_+9, b_+12);
  TAIL_S(incCutsceneState2);
}

void s_cutscene06Funce_hook(GB *gb) {
  BASE(cutscene06Funce);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_HL(wTmpcbb3);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x3c);
  CYC(b_+12, b_+15);
  TAIL_S(incCutsceneState2);
}

void s_cutscene06Funcf_hook(GB *gb) {
  BASE(cutscene06Funcf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decCbb3_hook, SYM(decCbb3), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, clearOam_hook, SYM(clearOam), b_+7);
  CALL_C(b_+7, cutscene_clearObjects_hook, SYM(cutscene_clearObjects), b_+10);
  CYC(b_+10, b_+12); A = 0x07; // CUTSCENE_S_DIN_IMPRISONED
  CYC(b_+12, b_+15); mem_wr(gb, wThreadStateBuffer + 0x0f, A); // wCutsceneIndex
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+19, b_+22); mem_wr(gb, wCutsceneState, A);
  CYC(b_+22, b_+24); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+24, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+27);
  CYC(b_+27, b_+30);
  TAIL(fadeoutToWhite);
}
