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

// Din imprisoned (CUTSCENE_S_DIN_IMPRISONED): Onox's castle outside and inside, the flash and
// the text, then on to the temple sinking. wTmpcfc0+$10 is the stage counter the Din-imprisoned
// event interactions advance; $cc1d (unnamed in the disassembly) holds the first one's index.
#define wcc1d (wFrameCounter + 0x1d)

void s_cutsceneDinImprisoned_hook(GB *gb) {
  BASE(cutsceneDinImprisoned);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wCutsceneState);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (intro_jump_table(gb));
    if (jt_ == b_+25) { goto state0; }
    else if (jt_ == b_+61) { goto state1; }
    else if (jt_ == b_+102) { goto state2; }
    else if (jt_ == b_+191) { goto state3; }
    else if (jt_ == b_+219) { goto state4; }
    else if (jt_ == b_+235) { goto state5; }
    else if (jt_ == b_+296) { goto state6; }
    else if (jt_ == b_+332) { goto state7; }
    else if (jt_ == b_+347) { goto state8; }
    else if (jt_ == b_+375) { goto state9; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+25, b_+28); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); A = 0x01;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+35); A = 0x09;
  CYC(b_+35, b_+38); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+38, b_+41); SET_HL(wTmpcbb3);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x58);
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x01);
  CYC(b_+46, b_+48); A = 0x09;
  CYC(b_+48, b_+50); B = 0x00;
  CALL_C(b_+50, s_seasonsFunc_03_7aa9_hook, SYM(seasonsFunc_03_7aa9), b_+53);
  CYC(b_+53, b_+55); A = 0x1c; // MUS_ONOX_CASTLE
  CALL_C(b_+55, playSound_b00_hook, SYM(playSound_b00), b_+58);
  CYC(b_+58, b_+61);
  TAIL(fadeinFromWhite);

state1:
  CYC(b_+61, b_+64); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+69); SET_HL(wTmpcbb3);
  CALL_C(b_+69, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+72);
  if (!(F & FZ)) { CYCT(b_+72, b_+74); goto scroll_up; }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+75); alu_xor(gb, A);
  CYC(b_+75, b_+78); mem_wr(gb, wGfxRegs1_SCY, A);
  CALL_C(b_+78, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+81);
  CYC(b_+81, b_+84);
  TAIL(fadeoutToWhite);
scroll_up:
  CYC(b_+84, b_+87); SET_HL(wTmpcbb3);
  CYC(b_+87, b_+88); A = mem_rd(gb, HL);
  CYC(b_+88, b_+90); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+94); SET_HL(wGfxRegs1_SCY);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+96); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+98); A = alu_dec8(gb, A);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  CYC(b_+99, b_+101); mem_wr(gb, hCameraY, A);
  RET(b_+101); return;

state2:
  CYC(b_+102, b_+105); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+105, b_+106); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+106); return; }
  CYC(b_+106, b_+107);
  CALL_C(b_+107, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+110);
  CYC(b_+110, b_+112); A = 0x0a;
  CYC(b_+112, b_+115); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CALL_C(b_+115, disableLcd_hook, SYM(disableLcd), b_+118);
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+122); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+122, b_+125); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+125, b_+127); A = 0x2e; // GFXH_SCENE_INSIDE_ONOX_CASTLE
  CALL_C(b_+127, loadGfxHeader_hook, SYM(loadGfxHeader), b_+130);
  CYC(b_+130, b_+132); A = 0x97; // PALH_SEASONS_97
  CALL_C(b_+132, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+135);
  CYC(b_+135, b_+137); A = 0x01;
  CYC(b_+137, b_+140); mem_wr(gb, wScrollMode, A);
  CYC(b_+140, b_+142); A = 0x18;
  CYC(b_+142, b_+145); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+145, loadAnimationData_hook, SYM(loadAnimationData), b_+148);
  CALL_C(b_+148, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+151);
  if (!(F & FZ)) { CYCT(b_+151, b_+153); goto objects_ready; }
  CYC(b_+151, b_+153);
  CYC(b_+153, b_+155); A = 0x4f; // INTERAC_DIN_IMPRISONED_EVENT
  CYC(b_+155, b_+156); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+156, b_+158); mem_wr(gb, HL, 0x00);
  CYC(b_+158, b_+161); mem_wr(gb, wcc1d, A);
  CALL_C(b_+161, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+164);
  if (!(F & FZ)) { CYCT(b_+164, b_+166); goto objects_ready; }
  CYC(b_+164, b_+166);
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x4f);
  CYC(b_+168, b_+169); L = alu_inc8(gb, L);
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x01);
objects_ready:
  CALL_C(b_+171, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+174);
  CYC(b_+174, b_+176); A = 0x0d;
  CALL_C(b_+176, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+179);
  CYC(b_+179, b_+182); SET_HL(wGfxRegs1_SCY);
  CYC(b_+182, b_+183); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+183, b_+185); mem_wr(gb, hCameraY, A);
  CYC(b_+185, b_+186); A = mem_rd(gb, HL);
  CYC(b_+186, b_+188); mem_wr(gb, hCameraX, A);
  CYC(b_+188, b_+191);
  TAIL(fadeinFromWhite);

state3:
  CYC(b_+191, b_+194); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+194, b_+195); A = mem_rd(gb, HL);
  CYC(b_+195, b_+197); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { RET_TAKEN(b_+197); return; }
  CYC(b_+197, b_+198);
  CYC(b_+198, b_+200); B = 0x04;
  for (;;) {
    CALL_C(b_+200, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+203);
    if (!(F & FZ)) { CYCT(b_+203, b_+205); break; }
    CYC(b_+203, b_+205);
    CYC(b_+205, b_+207); mem_wr(gb, HL, 0x4f); // INTERAC_DIN_IMPRISONED_EVENT
    CYC(b_+207, b_+208); L = alu_inc8(gb, L);
    CYC(b_+208, b_+210); mem_wr(gb, HL, 0x02);
    CYC(b_+210, b_+211); L = alu_inc8(gb, L);
    CYC(b_+211, b_+212); B = alu_dec8(gb, B);
    CYC(b_+212, b_+213); A = B;
    CYC(b_+213, b_+214); mem_wr(gb, HL, A);
    if (!(F & FZ)) { CYCT(b_+214, b_+216); continue; }
    CYC(b_+214, b_+216);
    break;
  }
  CYC(b_+216, b_+219);
  TAIL_S(incCutsceneState2);

state4:
  CYC(b_+219, b_+222); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+222, b_+224); alu_sub(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+224); return; }
  CYC(b_+224, b_+225);
  CYC(b_+225, b_+228); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+228, b_+229); A = alu_dec8(gb, A);
  CYC(b_+229, b_+232); mem_wr(gb, wTmpcbba, A);
  CYC(b_+232, b_+235);
  TAIL_S(incCutsceneState2);

state5:
  CYC(b_+235, b_+238); SET_HL(wTmpcbb3);
  CYC(b_+238, b_+240); B = 0x01;
  CALL_C(b_+240, flashScreen_hook, SYM(flashScreen), b_+243);
  if (F & FZ) { RET_TAKEN(b_+243); return; }
  CYC(b_+243, b_+244);
  CALL_C(b_+244, disableLcd_hook, SYM(disableLcd), b_+247);
  CYC(b_+247, b_+249); A = 0x01;
  CYC(b_+249, b_+251); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+251, b_+253); A = 0xfe;
  CYC(b_+253, b_+255); mem_wr(gb, hBgPaletteSources, A);
  CYC(b_+255, b_+257); A = 0x81;
  CALL_C(b_+257, s_seasonsFunc_03_7a6b_hook, SYM(seasonsFunc_03_7a6b), b_+260);
  CYC(b_+260, b_+262); A = 0x81;
  CYC(b_+262, b_+265); mem_wr(gb, wOpenedMenuType, A);
  CALL_C(b_+265, s_seasonsFunc_03_7a88_hook, SYM(seasonsFunc_03_7a88), b_+268);
  CYC(b_+268, b_+271); SET_BC(0x1e05); // TX_1e05
  CALL_C(b_+271, showText_hook, SYM(showText), b_+274);
  CYC(b_+274, b_+276); A = 0x0d;
  CALL_C(b_+276, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+279);
  CYC(b_+279, b_+282); SET_HL(wGfxRegs1_SCY);
  CYC(b_+282, b_+283); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+283, b_+285); mem_wr(gb, hCameraY, A);
  CYC(b_+285, b_+286); A = mem_rd(gb, HL);
  CYC(b_+286, b_+288); mem_wr(gb, hCameraX, A);
  CYC(b_+288, b_+291); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+291, b_+293); mem_wr(gb, HL, 0x0d);
  CYC(b_+293, b_+296);
  TAIL_S(incCutsceneState2);

state6:
  CALL_C(b_+296, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+299);
  CALL_C(b_+299, disableLcd_hook, SYM(disableLcd), b_+302);
  CYC(b_+302, b_+304); A = 0x0e; // UNCMP_GFXH_0e
  CALL_C(b_+304, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+307);
  CYC(b_+307, b_+309); A = 0x50; // SND_CLINK
  CALL_C(b_+309, playSound_b00_hook, SYM(playSound_b00), b_+312);
  CYC(b_+312, b_+314); A = 0x0d;
  CALL_C(b_+314, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+317);
  CALL_C(b_+317, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+320);
  CYC(b_+320, b_+323); SET_HL(wTmpcbb3);
  CYC(b_+323, b_+325); mem_wr(gb, HL, 0xf0);
  CYC(b_+325, b_+326); alu_xor(gb, A);
  CYC(b_+326, b_+329); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+329, b_+332);
  TAIL_S(incCutsceneState2);

state7:
  CYC(b_+332, b_+335); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+335, b_+336); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+336); return; }
  CYC(b_+336, b_+337);
  CALL_C(b_+337, decCbb3_hook, SYM(decCbb3), b_+340);
  if (!(F & FZ)) { RET_TAKEN(b_+340); return; }
  CYC(b_+340, b_+341);
  CALL_C(b_+341, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+344);
  CYC(b_+344, b_+347);
  TAIL(fadeoutToWhite);

state8:
  CYC(b_+347, b_+350); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+350, b_+351); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+351); return; }
  CYC(b_+351, b_+352);
  CALL_C(b_+352, s_incCutsceneState2_hook, SYM(incCutsceneState2), b_+355);
  CYC(b_+355, b_+357); A = 0xff;
  CYC(b_+357, b_+360); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+360, b_+362); A = 0x0e;
  CYC(b_+362, b_+365); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+365, b_+367); A = 0x07;
  CYC(b_+367, b_+369); B = 0x01;
  CALL_C(b_+369, s_seasonsFunc_03_7aa9_hook, SYM(seasonsFunc_03_7aa9), b_+372);
  CYC(b_+372, b_+375);
  TAIL(fadeinFromWhite);

state9:
  CYC(b_+375, b_+378); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+378, b_+379); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+379); return; }
  CYC(b_+379, b_+380);
  CYC(b_+380, b_+383); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+383, b_+384); A = mem_rd(gb, HL);
  CYC(b_+384, b_+386); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+386); return; }
  CYC(b_+386, b_+387);
  CALL_C(b_+387, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+390);
  CYC(b_+390, b_+392); A = 0x08; // CUTSCENE_S_TEMPLE_SINKING
  CYC(b_+392, b_+395); mem_wr(gb, wThreadStateBuffer + 0x0f, A); // wCutsceneIndex
  CYC(b_+395, b_+396); alu_xor(gb, A);
  CYC(b_+396, b_+399); mem_wr(gb, wCutsceneState, A);
  CYC(b_+399, b_+402);
  TAIL(fadeoutToWhite);
}

// Fills VRAM bank 1's tile map at $9800 with a (attributes) and clears bank 0's.
void s_seasonsFunc_03_7a6b_hook(GB *gb) {
  BASE(seasonsFunc_03_7a6b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); mem_wr(gb, IO_VBK, A);
  CYC(b_+6, b_+9); SET_HL(0x9800);
  CYC(b_+9, b_+12); SET_BC(0x0400);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8B);
  CALL_C(b_+14, fillMemoryBc_hook, SYM(fillMemoryBc), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); mem_wr(gb, IO_VBK, A);
  CYC(b_+20, b_+23); SET_HL(0x9800);
  CYC(b_+23, b_+26); SET_BC(0x0400);
  CYC(b_+26, b_+29);
  TAIL(clearMemoryBc);
}

// Same for the WRAM bank 4 tile map buffers: $d000 cleared, $d400 filled with a.
void s_seasonsFunc_03_7a88_hook(GB *gb) {
  BASE(seasonsFunc_03_7a88);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+4); A = mem_rd(gb, IO_SVBK);
  CYC(b_+4, b_+5); push_effect(gb, AF);
  CYC(b_+5, b_+7); A = 0x04;
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  CYC(b_+9, b_+12); SET_HL(w4RandomBuffer);
  CYC(b_+12, b_+15); SET_BC(0x0240);
  CALL_C(b_+15, clearMemoryBc_hook, SYM(clearMemoryBc), b_+18);
  CYC(b_+18, b_+21); SET_HL(w4RandomBuffer + 0x400);
  CYC(b_+21, b_+24); SET_BC(0x0240);
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8B);
  CALL_C(b_+26, fillMemoryBc_hook, SYM(fillMemoryBc), b_+29);
  CYC(b_+29, b_+30); SET_AF(pop_effect(gb));
  CYC(b_+30, b_+32); mem_wr(gb, IO_SVBK, A);
  RET(b_+32); return;
}

// Loads the outside-Onox-castle scene: a = gfx register state index, b = INTERAC_88 subid.
void s_seasonsFunc_03_7aa9_hook(GB *gb) {
  BASE(seasonsFunc_03_7aa9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); D = A;
  CYC(b_+1, b_+2); A = B;
  CYC(b_+2, b_+3); E = A;
  CALL_C(b_+3, disableLcd_hook, SYM(disableLcd), b_+6);
  CYC(b_+6, b_+7); push_effect(gb, DE);
  CYC(b_+7, b_+9); A = 0x2f; // GFXH_SCENE_OUTSIDE_ONOX_CASTLE
  CALL_C(b_+9, loadGfxHeader_hook, SYM(loadGfxHeader), b_+12);
  CYC(b_+12, b_+14); A = 0x0f; // PALH_0f
  CALL_C(b_+14, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+17);
  CYC(b_+17, b_+19); A = 0x3b; // PALH_TILESET_ONOX_CASTLE_OUTSIDE_WINTER
  CALL_C(b_+19, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+22);
  CYC(b_+22, b_+23); SET_DE(pop_effect(gb));
  CALL_C(b_+23, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto load_regs; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x88); // INTERAC_88
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); mem_wr(gb, HL, E);
load_regs:
  CYC(b_+32, b_+33); A = D;
  CALL_C(b_+33, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+36);
  CYC(b_+36, b_+39); SET_HL(wGfxRegs1_SCY);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+42); mem_wr(gb, hCameraY, A);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); mem_wr(gb, hCameraX, A);
  RET(b_+45); return;
}
