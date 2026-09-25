#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bank3_jump_table(GB *gb) {
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

// ref/oracles-disasm/code/bank3Cutscenes.s, the Seasons-only pieces. The shared states are in
// src/game/bank3Cutscenes.c.

// Seasons splits Ages' introCinematic_inTemple_state1 in two: state 1 only waits for the
// palette fade, this state walks Link up to the triforce on simulated input.
void s_introCinematic_inTemple_state1_5_hook(GB *gb) {
  BASE(introCinematic_inTemple_state1_5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+7); TAIL(introCinematic_inTemple_updateCamera); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(b_+11, introCinematic_inTemple_updateCamera_hook, SYM(introCinematic_inTemple_updateCamera), b_+14);
  CYC(b_+14, b_+17);
  TAIL(intro_incState);
}

// Draw the sprites that complement the image of Link on the horse (the 1st image)
void s_introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGfxRegs2_SCY);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_cpl(gb);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_cpl(gb);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+14); mem_wr(gb, hOamTail, A);
  CYC(b_+14, b_+17); SET_HL(SYM(linkOnHorseCloseupSprites_1));
  CYC(b_+17, b_+20);
  TAIL(addSpritesToOam_withOffset);
}

// seasonsFunc_03_5367@func
static void bank3Cutscenes_d_func(GB *gb) {
  BASE(seasonsFunc_03_5367);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcbb6);
  CYC(b_+28, b_+29); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto L_5394; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wTmpcbba);
  CYC(b_+34, b_+36); alu_xor(gb, 0x01);
  CYC(b_+36, b_+39); mem_wr(gb, wTmpcbba, A);
  CYC(b_+39, b_+41); A = 0x05;
  if (F & FZ) { CYCT(b_+41, b_+43); goto L_5394; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x22;
L_5394:
  CYC(b_+45, b_+48); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+48, b_+51); A = mem_rd(gb, wTmpcbba);
  CYC(b_+51, b_+52); alu_or(gb, A);
  RET(b_+52); return;
}

void s_seasonsFunc_03_5367_hook(GB *gb) {
  BASE(seasonsFunc_03_5367);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, bank3Cutscenes_d_func, b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0506);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_5372; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_BC(0x0000);
L_5372:
  CYC(b_+11, b_+14); SET_HL(wTmpcbbb);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+19); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+24); mem_wr(gb, wGfxRegs2_SCX, A);
  RET(b_+24); return;
}

void s_introCinematic_inTemple_state1_hook(GB *gb) {
  BASE(introCinematic_inTemple_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  TAIL(intro_incState);
}

void s_introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hOamTail, A);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); SET_HL(SYM(linkOnHorseCloseupSprites_2));
  CYC(b_+13, b_+16);
  TAIL(addSpritesToOam_withOffset);
}

void s_introCinematic_ridingHorse_drawTempleSprites_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawTempleSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hOamTail, A);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+7, b_+8); alu_cpl(gb);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); SET_HL(SYM(introTempleSprites));
  CYC(b_+13, b_+16);
  TAIL(addSpritesToOam_withOffset);
}

void s_introCinematic_ridingHorse_state0_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_disableLcd, SYM(disableLcd), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTextIsActive);
  CYC(b_+6, b_+9); SET_BC(0x0460);
  CALL_C(b_+9, s_clearMemoryBc, SYM(clearMemoryBc), b_+12);
  CYC(b_+12, b_+14); A = 0x10;
  CYC(b_+14, b_+16); mem_wr(gb, hOamTail, A);
  CYC(b_+16, b_+18); A = 0x9b;
  CALL_C(b_+18, s_loadGfxHeader, SYM(loadGfxHeader), b_+21);
  CYC(b_+21, b_+23); A = 0x90;
  CALL_C(b_+23, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+26);
  CYC(b_+26, b_+29); SET_HL(wTmpcbb3);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x7e);
  CYC(b_+31, b_+32); SET_HL(HL + 1);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x03);
  CYC(b_+34, b_+36); A = 0x20;
  CYC(b_+36, b_+39); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+39, b_+41); A = 0x10;
  CYC(b_+41, b_+44); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+44, b_+46); A = 0x22;
  CYC(b_+46, b_+49); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+49, b_+51); A = 0x01;
  CYC(b_+51, b_+54); mem_wr(gb, wTmpcbba, A);
  CYC(b_+54, b_+56); A = 0x08;
  CALL_C(b_+56, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+59);
  CYC(b_+59, b_+61); A = 0x3f;
  CALL_C(b_+61, s_playSound, SYM(playSound_b00), b_+64);
  CALL_C(b_+64, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+67);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto fadeIn; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x75);
  CYC(b_+71, b_+72); L = alu_inc8(gb, L);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x00);
fadeIn:
  CYC(b_+74, b_+76); A = 0x14;
  CALL_C(b_+76, s_fadeinFromWhiteWithDelay, SYM(fadeinFromWhiteWithDelay), b_+79);
  CYC(b_+79, b_+82); SET_HL(wLockBG7Color3ToBlack);
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x01);
  CYC(b_+84, b_+87);
  TAIL(intro_incState);
}

void s_introCinematic_ridingHorse_state1_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_introCinematic_moveBlackBarsIn, SYM(introCinematic_moveBlackBarsIn), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb3);
  CALL_C(b_+6, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, s_clearPaletteFadeVariablesAndRefreshPalettes, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+13);
  CYC(b_+13, b_+15); A = 0x96;
  CALL_C(b_+15, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+18);
  CYC(b_+18, b_+20); A = 0x0c;
  CALL_C(b_+20, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wGfxRegs2_SCY);
  CYC(b_+26, b_+29); mem_wr(gb, wTmpcbbb, A);
  CYC(b_+29, b_+32); A = mem_rd(gb, wGfxRegs2_SCX);
  CYC(b_+32, b_+35); mem_wr(gb, wTmpcbbc, A);
  CALL_C(b_+35, s_introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1_hook, SYM(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1), b_+38);
  CYC(b_+38, b_+41); SET_HL(wTmpcbb3);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x58);
  CYC(b_+43, b_+44); SET_HL(HL + 1);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x01);
  CYC(b_+46, b_+49);
  TAIL(intro_incState);
}

void s_introCinematic_ridingHorse_state2_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CALL_C(b_+3, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto drawSprites; }
  CYC(b_+6, b_+8);
  CALL_C(b_+8, s_disableLcd, SYM(disableLcd), b_+11);
  CYC(b_+11, b_+13); A = 0x92;
  CALL_C(b_+13, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+16);
  CYC(b_+16, b_+18); A = 0x9c;
  CALL_C(b_+18, s_loadGfxHeader, SYM(loadGfxHeader), b_+21);
  CYC(b_+21, b_+23); A = 0x0a;
  CALL_C(b_+23, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+26);
  CALL_C(b_+26, s_intro_incState, SYM(intro_incState), b_+29);
  CYC(b_+29, b_+31);
  TAIL_S(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
drawSprites:
  CALL_C(b_+31, s_seasonsFunc_03_5367_hook, SYM(seasonsFunc_03_5367), b_+34);
  s_introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1_hook(gb); return; // falls through
}

void s_endgameCutsceneHandler_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto runHandler; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+11); SET_HL(wTmpcbb3);
  CYC(b_+11, b_+13); B = 0x10;
  CALL_C(b_+13, s_clearMemory, SYM(clearMemory), b_+16);
runHandler:
  CYC(b_+16, b_+17); A = E;
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (bank3_jump_table(gb));
    if (jt_ == SYM(endgameCutsceneHandler_09) && hook_is(gb, SYM(endgameCutsceneHandler_09), s_endgameCutsceneHandler_09)) { s_endgameCutsceneHandler_09(gb); return; }
    if (jt_ == SYM(endgameCutsceneHandler_0a) && hook_is(gb, SYM(endgameCutsceneHandler_0a), s_endgameCutsceneHandler_0a)) { s_endgameCutsceneHandler_0a(gb); return; }
    if (jt_ == SYM(endgameCutsceneHandler_0f) && hook_is(gb, SYM(endgameCutsceneHandler_0f), s_endgameCutsceneHandler_0f_hook)) { s_endgameCutsceneHandler_0f_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}
