#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/linkedGameCutscenes.s, bank $03: the linked-game
// cutscenes CUTSCENE_S_FLAME_OF_DESTRUCTION, CUTSCENE_S_ZELDA_VILLAGERS and
// CUTSCENE_S_ZELDA_KIDNAPPED. wGenericCutscene_cbb3 is the step, cbb4 the flash sub-step, cbb5
// a countdown; wTmpcfc0 bits are the handshake with the cutscene's interactions.

static uint16_t linked_jump_table(GB *gb) {
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

static void linked_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void s_flameOfDestructionCutsceneBody_hook(GB *gb) {
  BASE(flameOfDestructionCutsceneBody);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == SYM(bank3Cutscene_state0) && hook_is(gb, SYM(bank3Cutscene_state0), s_bank3Cutscene_state0_hook)) { s_bank3Cutscene_state0_hook(gb); return; }
    else if (jt_ == SYM(flameOfDestructionCutscene_state1) && hook_is(gb, SYM(flameOfDestructionCutscene_state1), s_flameOfDestructionCutscene_state1_hook)) { s_flameOfDestructionCutscene_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void s_zeldaAndVillagersCutsceneBody_hook(GB *gb) {
  BASE(zeldaAndVillagersCutsceneBody);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == SYM(bank3Cutscene_state0) && hook_is(gb, SYM(bank3Cutscene_state0), s_bank3Cutscene_state0_hook)) { s_bank3Cutscene_state0_hook(gb); return; }
    else if (jt_ == SYM(zeldaAndVillagersCutscene_state1) && hook_is(gb, SYM(zeldaAndVillagersCutscene_state1), s_zeldaAndVillagersCutscene_state1_hook)) { s_zeldaAndVillagersCutscene_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void s_zeldaKidnappedCutsceneBody_hook(GB *gb) {
  BASE(zeldaKidnappedCutsceneBody);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == SYM(bank3Cutscene_state0) && hook_is(gb, SYM(bank3Cutscene_state0), s_bank3Cutscene_state0_hook)) { s_bank3Cutscene_state0_hook(gb); return; }
    else if (jt_ == SYM(zeldaKidnappedCutscene_state1) && hook_is(gb, SYM(zeldaKidnappedCutscene_state1), s_zeldaKidnappedCutscene_state1_hook)) { s_zeldaKidnappedCutscene_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Clears the cutscene variables and bank 1, enables objects, disables the menu.
void s_bank3Cutscene_state0_hook(GB *gb) {
  BASE(bank3Cutscene_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x10;
  CYC(b_+2, b_+5); SET_HL(wGenericCutscene_cbb3);
  CALL_C(b_+5, s_clearMemory, SYM(clearMemory), b_+8);
  CALL_C(b_+8, s_clearWramBank1, SYM(clearWramBank1), b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+17); A = 0x80;
  CYC(b_+17, b_+20); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); mem_wr(gb, wCutsceneState, A);
  RET(b_+25); return;
}

// flameOfDestructionCutscene_state1@fadeInAndLightTorch: fast fade in of the sprite palettes
// in b (the background ones stay), and the torch sound.
static void linked_fade_in_and_light_torch(GB *gb) {
  BASE(flameOfDestructionCutscene_state1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+142, s_fastFadeinFromBlack, SYM(fastFadeinFromBlack), b_+145);
  CYC(b_+145, b_+146); A = B;
  CYC(b_+146, b_+149); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+149, b_+152); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+152, b_+153); alu_xor(gb, A);
  CYC(b_+153, b_+156); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+156, b_+159); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+159, b_+161); A = 0x72; // SND_LIGHTTORCH
  CYC(b_+161, b_+164);
  TAIL(playSound_b00);
}

// The Room of Rites: lightning, Twinrova's flames, the torch, the Flame of Destruction's
// text, then the warp to the D5 entrance.
void s_flameOfDestructionCutscene_state1_hook(GB *gb) {
  BASE(flameOfDestructionCutscene_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == b_+28) { goto fadeToBlack; }
    else if (jt_ == b_+39) { goto roomOfRitesStart; }
    else if (jt_ == b_+80) { goto flashScreen; }
    else if (jt_ == b_+95) { goto changePalettes; }
    else if (jt_ == b_+127) { goto startCutsceneText08; }
    else if (jt_ == b_+164) { goto startFadeInAndLightTorch; }
    else if (jt_ == b_+181) { goto createSomeObjects; }
    else if (jt_ == b_+222) { goto startCutsceneText09; }
    else if (jt_ == b_+231) { goto startCutsceneText0a; }
    else if (jt_ == b_+240) { goto startCutsceneText0b; }
    else if (jt_ == b_+249) { goto startCutsceneText0c; }
    else if (jt_ == b_+264) { goto finish; }
    else { HANDOFF(HL); }
  } while (0);

fadeToBlack:
  CYC(b_+28, b_+30); A = 0x28;
  CYC(b_+30, b_+33); mem_wr(gb, wGenericCutscene_cbb5, A);
  CALL_C(b_+33, s_fastFadeoutToBlack, SYM(fastFadeoutToBlack), b_+36);
  CYC(b_+36, b_+39);
  s_incTmpcbb3_hook(gb); return;

roomOfRitesStart:
  CALL_C(b_+39, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+42);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CALL_C(b_+43, s_bank3CutsceneLoadRoomOfRites_hook, SYM(bank3CutsceneLoadRoomOfRites), b_+46);
  CALL_C(b_+46, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); goto torch; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
  CYC(b_+53, b_+54); L = alu_inc8(gb, L);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x03);
torch:
  CYC(b_+56, b_+58); A = 0x13;
  CALL_C(b_+58, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+61);
  CYC(b_+61, b_+63); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+63, playSound_b00_hook, SYM(playSound_b00), b_+66);
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CYC(b_+67, b_+70); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+70, b_+73); mem_wr(gb, wGenericCutscene_cbb6, A);
  CYC(b_+73, b_+74); A = alu_dec8(gb, A);
  CYC(b_+74, b_+77); mem_wr(gb, wGenericCutscene_cbba, A);
  CALL_C(b_+77, s_incTmpcbb3_hook, SYM(incTmpcbb3), b_+80);
flashScreen:
  CYC(b_+80, b_+83); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+83, b_+85); B = 0x04;
  CALL_C(b_+85, s_flashScreen, SYM(flashScreen), b_+88);
  if (F & FZ) { RET_TAKEN(b_+88); return; }
  CYC(b_+88, b_+89);
  CALL_C(b_+89, s_clearPaletteFadeVariablesAndRefreshPalettes, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+92);
  CYC(b_+92, b_+95);
  s_incTmpcbb3_hook(gb); return;

changePalettes:
  CALL_C(b_+95, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto stopMusic; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x04);
stopMusic:
  CYC(b_+105, b_+107); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+107, playSound_b00_hook, SYM(playSound_b00), b_+110);
  CYC(b_+110, b_+112); A = 0x04;
  CYC(b_+112, b_+115); mem_wr(gb, wGenericCutscene_cbb5, A);
  CALL_C(b_+115, s_clearFadingPalettes2, SYM(clearFadingPalettes2), b_+118);
  CYC(b_+118, b_+120); A = 0xef;
  CYC(b_+120, b_+122); mem_wr(gb, hSprPaletteSources, A);
  CYC(b_+122, b_+124); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+124, b_+127);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText08:
  CYC(b_+127, b_+130); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+130, b_+131); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+134); A = 0x04;
  CYC(b_+134, b_+137); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+137, b_+139); C = 0x08; // TX_5008
  CYC(b_+139, b_+142);
  s_showCutscene50xxText_hook(gb); return;

startFadeInAndLightTorch:
  CALL_C(b_+164, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+167);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; }
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+170); B = 0x40;
  CALL_L(b_+170, linked_fade_in_and_light_torch, b_+173);
  CYC(b_+173, b_+175); A = 0x1e;
  CYC(b_+175, b_+178); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+178, b_+181);
  s_incTmpcbb3_hook(gb); return;

createSomeObjects:
  CALL_C(b_+181, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+184);
  if (!(F & FZ)) { RET_TAKEN(b_+184); return; }
  CYC(b_+184, b_+185);
  CALL_C(b_+185, s_fadeinFromBlack, SYM(fadeinFromBlack), b_+188);
  CYC(b_+188, b_+190); A = 0xaf;
  CYC(b_+190, b_+193); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+193, b_+196); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+196, b_+197); alu_xor(gb, A);
  CYC(b_+197, b_+200); mem_wr(gb, wTmpcfc0 + 0x06, A);
  CALL_C(b_+200, s_cutscene_func_03_72af_hook, SYM(cutscene_func_03_72af), b_+203);
  CALL_C(b_+203, s_loadInteracIdb4_subid6And7_hook, SYM(loadInteracIdb4_subid6And7), b_+206);
  CYC(b_+206, b_+208); A = 0x21; // MUS_DISASTER
  CYC(b_+208, b_+211); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+211, playSound_b00_hook, SYM(playSound_b00), b_+214);
  CYC(b_+214, b_+216); A = 0x1e;
  CYC(b_+216, b_+219); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+219, b_+222);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText09:
  CALL_C(b_+222, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+225);
  if (!(F & FZ)) { RET_TAKEN(b_+225); return; }
  CYC(b_+225, b_+226);
  CYC(b_+226, b_+228); C = 0x09; // TX_5009
  CYC(b_+228, b_+231);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText0a:
  CALL_C(b_+231, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+234);
  if (!(F & FZ)) { RET_TAKEN(b_+234); return; }
  CYC(b_+234, b_+235);
  CYC(b_+235, b_+237); C = 0x0a; // TX_500a
  CYC(b_+237, b_+240);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText0b:
  CALL_C(b_+240, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+243);
  if (!(F & FZ)) { RET_TAKEN(b_+243); return; }
  CYC(b_+243, b_+244);
  CYC(b_+244, b_+246); C = 0x0b; // TX_500b
  CYC(b_+246, b_+249);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText0c:
  CALL_C(b_+249, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+252);
  if (!(F & FZ)) { RET_TAKEN(b_+252); return; }
  CYC(b_+252, b_+253);
  CYC(b_+253, b_+255); C = 0x0c; // TX_500c
  CALL_C(b_+255, s_showCutscene50xxText_hook, SYM(showCutscene50xxText), b_+258);
  CYC(b_+258, b_+260); A = 0x3c;
  CYC(b_+260, b_+263); mem_wr(gb, wGenericCutscene_cbb5, A);
  RET(b_+263); return;

finish:
  CALL_C(b_+264, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+267);
  if (!(F & FZ)) { RET_TAKEN(b_+267); return; }
  CYC(b_+267, b_+268);
  CYC(b_+268, b_+269); alu_xor(gb, A);
  CYC(b_+269, b_+272); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+272, b_+274); A = 0x20; // GLOBALFLAG_FLAMES_OF_DESTRUCTION_SEEN
  CALL_C(b_+274, s_setGlobalFlag, SYM(setGlobalFlag), b_+277);
  CYC(b_+277, b_+279); A = 0x03;
  CYC(b_+279, b_+282); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+282, b_+285); SET_HL(b_+293); // @warpDest
  CALL_C(b_+285, s_setWarpDestVariables, SYM(setWarpDestVariables), b_+288);
  CYC(b_+288, b_+290); A = 0x0f; // PALH_0f
  CYC(b_+290, b_+293);
  TAIL(loadPaletteHeader);
}

// Zelda with the villagers: outside Impa's house, then the Sokra screen, then the warp into
// the first room of D8.
void s_zeldaAndVillagersCutscene_state1_hook(GB *gb) {
  BASE(zeldaAndVillagersCutscene_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == b_+18) { goto start; }
    else if (jt_ == b_+24) { goto loadImpaRoomAndMusic; }
    else if (jt_ == b_+65) { goto waitUntilFadeInDone; }
    else if (jt_ == b_+73) { goto waitToFadeOut; }
    else if (jt_ == b_+85) { goto loadSokraRoomAndMusic; }
    else if (jt_ == b_+120) { goto waitUntilFadeInDone2; }
    else if (jt_ == b_+130) { goto finish; }
    else { HANDOFF(HL); }
  } while (0);

start:
  CALL_C(b_+18, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+21);
  CYC(b_+21, b_+24);
  s_incTmpcbb3_hook(gb); return;

loadImpaRoomAndMusic:
  CYC(b_+24, b_+27); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); A = 0x03;
  CYC(b_+31, b_+34); SET_BC(0x00b6); // ROOM_SEASONS_0b6
  CALL_C(b_+34, s_disableLcdAndLoadRoom_body, SYM(disableLcdAndLoadRoom_body), b_+37);
  CYC(b_+37, b_+39); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+44); A = 0x20; // MUS_TRIUMPHANT
  CYC(b_+44, b_+47); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+47, playSound_b00_hook, SYM(playSound_b00), b_+50);
  CYC(b_+50, b_+52); A = 0x02;
  CALL_C(b_+52, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+55);
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CALL_C(b_+56, s_loadGroupOfInteractions_hook, SYM(loadGroupOfInteractions), b_+59);
  CALL_C(b_+59, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+62);
  CYC(b_+62, b_+65);
  s_incTmpcbb3_hook(gb); return;

waitUntilFadeInDone:
  CYC(b_+65, b_+68); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73);
  s_incTmpcbb3_hook(gb); return;

waitToFadeOut:
  CYC(b_+73, b_+76); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+76, b_+78); alu_bit(gb, 1, A);
  if (F & FZ) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CALL_C(b_+79, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+82);
  CYC(b_+82, b_+85);
  s_incTmpcbb3_hook(gb); return;

loadSokraRoomAndMusic:
  CYC(b_+85, b_+88); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+93); SET_BC(0x00e9); // ROOM_SEASONS_0e9
  CALL_C(b_+93, s_disableLcdAndLoadRoom_body, SYM(disableLcdAndLoadRoom_body), b_+96);
  CYC(b_+96, b_+98); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+98, playSound_b00_hook, SYM(playSound_b00), b_+101);
  CYC(b_+101, b_+103); A = 0x02;
  CALL_C(b_+103, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+106);
  CALL_C(b_+106, s_clearWramBank1, SYM(clearWramBank1), b_+109);
  CYC(b_+109, b_+111); A = 0x01;
  CALL_C(b_+111, s_loadGroupOfInteractions_hook, SYM(loadGroupOfInteractions), b_+114);
  CALL_C(b_+114, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+117);
  CYC(b_+117, b_+120);
  s_incTmpcbb3_hook(gb); return;

waitUntilFadeInDone2:
  CYC(b_+120, b_+123); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+123, b_+124); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+124); return; }
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+127); C = 0x10; // TX_5010
  CYC(b_+127, b_+130);
  s_showCutscene50xxText_hook(gb); return;

finish:
  CALL_C(b_+130, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+133);
  if (!(F & FZ)) { RET_TAKEN(b_+133); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+135); alu_xor(gb, A);
  CYC(b_+135, b_+138); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+138, b_+140); A = 0x1e; // GLOBALFLAG_ZELDA_VILLAGERS_SEEN
  CALL_C(b_+140, s_setGlobalFlag, SYM(setGlobalFlag), b_+143);
  CYC(b_+143, b_+146); SET_HL(b_+149); // @warpDest
  CYC(b_+146, b_+149);
  TAIL(setWarpDestVariables);
}

// The handler below, with the status bar kept up to date until step $10.
void s_zeldaKidnappedCutscene_state1_hook(GB *gb) {
  BASE(zeldaKidnappedCutscene_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_zeldaKidnappedCutscene_state1Handler_hook, SYM(zeldaKidnappedCutscene_state1Handler), b_+3);
  CYC(b_+3, b_+6); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+9, b_+12); TAIL(updateStatusBar); }
  CYC(b_+9, b_+12);
  RET(b_+12); return;
}

// Zelda kidnapped: the Sokra screen with Zelda and the villagers, Twinrova's arrival and the
// flash, then the Room of Rites and the warp towards Onox.
void s_zeldaKidnappedCutscene_state1Handler_hook(GB *gb) {
  BASE(zeldaKidnappedCutscene_state1Handler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == b_+46) { goto startByFadingOut; }
    else if (jt_ == b_+52) { goto loadSokraRoomAndInteractions; }
    else if (jt_ == b_+87) { goto waitUntilRoomLoaded; }
    else if (jt_ == b_+104) { goto startCutsceneText11; }
    else if (jt_ == b_+120) { goto func4; }
    else if (jt_ == b_+148) { goto func5; }
    else if (jt_ == b_+179) { goto startCutsceneText12; }
    else if (jt_ == b_+195) { goto startCutsceneText13; }
    else if (jt_ == b_+204) { goto startCutsceneText14; }
    else if (jt_ == b_+213) { goto func9; }
    else if (jt_ == b_+225) { goto funca; }
    else if (jt_ == b_+243) { goto funcb; }
    else if (jt_ == b_+275) { goto startCutsceneText16; }
    else if (jt_ == b_+284) { goto startCutsceneText17; }
    else if (jt_ == b_+293) { goto funce; }
    else if (jt_ == b_+315) { goto funcf; }
    else if (jt_ == b_+326) { goto loadRoomOfRitesAndInteractions; }
    else if (jt_ == b_+372) { goto startCutsceneText18; }
    else if (jt_ == b_+381) { goto startCutsceneText19; }
    else if (jt_ == b_+390) { goto startCutsceneText1a; }
    else if (jt_ == b_+399) { goto finish; }
    else { HANDOFF(HL); }
  } while (0);

startByFadingOut:
  CALL_C(b_+46, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+49);
  CYC(b_+49, b_+52);
  s_incTmpcbb3_hook(gb); return;

loadSokraRoomAndInteractions:
  CYC(b_+52, b_+55); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+55, b_+56); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+56); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+60); SET_BC(0x00e9); // ROOM_SEASONS_0e9
  CALL_C(b_+60, s_disableLcdAndLoadRoom_body, SYM(disableLcdAndLoadRoom_body), b_+63);
  CYC(b_+63, b_+65); A = 0x02;
  CALL_C(b_+65, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+68);
  CALL_C(b_+68, s_restartSound, SYM(restartSound), b_+71);
  CYC(b_+71, b_+73); A = 0x02;
  CALL_C(b_+73, s_loadGroupOfInteractions_hook, SYM(loadGroupOfInteractions), b_+76);
  CALL_C(b_+76, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+79);
  CYC(b_+79, b_+81); A = 0x3c;
  CYC(b_+81, b_+84); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+84, b_+87);
  s_incTmpcbb3_hook(gb); return;

waitUntilRoomLoaded:
  CALL_C(b_+87, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+90);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+94); SET_HL(wTmpcfc0);
  CYC(b_+94, b_+96); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+96, b_+98); A = 0xff;
  CYC(b_+98, b_+101); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+101, b_+104);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText11:
  CYC(b_+104, b_+107); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+107, b_+108); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+111); C = 0x11; // TX_5011
  CALL_C(b_+111, s_showCutscene50xxText_hook, SYM(showCutscene50xxText), b_+114);
  CYC(b_+114, b_+116); A = 0x5a;
  CYC(b_+116, b_+119); mem_wr(gb, wGenericCutscene_cbb5, A);
  RET(b_+119); return;

func4:
  CALL_C(b_+120, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+123);
  if (F & FZ) { CYCT(b_+123, b_+125); goto textDone; }
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); A = 0x3c;
  CYC(b_+127, b_+128); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+128); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+132); SET_HL(wTmpcfc0);
  CYC(b_+132, b_+134); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  RET(b_+134); return;
textDone:
  CYC(b_+135, b_+138); SET_HL(wTmpcfc0);
  CYC(b_+138, b_+140); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+140, b_+142); A = 0x3c;
  CYC(b_+142, b_+145); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+145, b_+148);
  s_incTmpcbb3_hook(gb); return;

func5:
  CYC(b_+148, b_+151); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+151, b_+152); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+152); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); A = 0x1e;
  CYC(b_+155, b_+158); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+158, b_+159); alu_xor(gb, A);
  CYC(b_+159, b_+162); mem_wr(gb, wTmpcfc0 + 0x06, A);
  CALL_C(b_+162, s_cutscene_func_03_72af_hook, SYM(cutscene_func_03_72af), b_+165);
  CALL_C(b_+165, s_loadInteracIdb4_subid2And3_hook, SYM(loadInteracIdb4_subid2And3), b_+168);
  CYC(b_+168, b_+170); A = 0x21; // MUS_DISASTER
  CYC(b_+170, b_+173); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+173, playSound_b00_hook, SYM(playSound_b00), b_+176);
  CYC(b_+176, b_+179);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText12:
  CYC(b_+179, b_+182); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+182, b_+184); alu_bit(gb, 0, A);
  if (F & FZ) { RET_TAKEN(b_+184); return; }
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+188); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+188, b_+189); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+189); return; }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+192); C = 0x12; // TX_5012
  CYC(b_+192, b_+195);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText13:
  CALL_C(b_+195, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+198);
  if (!(F & FZ)) { RET_TAKEN(b_+198); return; }
  CYC(b_+198, b_+199);
  CYC(b_+199, b_+201); C = 0x13; // TX_5013
  CYC(b_+201, b_+204);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText14:
  CALL_C(b_+204, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+207);
  if (!(F & FZ)) { RET_TAKEN(b_+207); return; }
  CYC(b_+207, b_+208);
  CYC(b_+208, b_+210); C = 0x14; // TX_5014
  CYC(b_+210, b_+213);
  s_showCutscene50xxText_hook(gb); return;

func9:
  CALL_C(b_+213, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+216);
  if (!(F & FZ)) { RET_TAKEN(b_+216); return; }
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+220); SET_HL(wTmpcfc0);
  CYC(b_+220, b_+222); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+222, b_+225);
  s_incTmpcbb3_hook(gb); return;

funca:
  CYC(b_+225, b_+228); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+228, b_+230); alu_bit(gb, 0, A);
  if (F & FZ) { RET_TAKEN(b_+230); return; }
  CYC(b_+230, b_+231);
  CYC(b_+231, b_+232); alu_xor(gb, A);
  CYC(b_+232, b_+235); mem_wr(gb, wGenericCutscene_cbb4, A);
  CYC(b_+235, b_+237); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+237, playSound_b00_hook, SYM(playSound_b00), b_+240);
  CALL_C(b_+240, s_incTmpcbb3_hook, SYM(incTmpcbb3), b_+243);
funcb:
  CALL_C(b_+243, s_zeldaKidnappedFlashFadeoutToWhite_hook, SYM(zeldaKidnappedFlashFadeoutToWhite), b_+246);
  if (!(F & FZ)) { RET_TAKEN(b_+246); return; }
  CYC(b_+246, b_+247);
  CALL_C(b_+247, s_clearWramBank1, SYM(clearWramBank1), b_+250);
  CYC(b_+250, b_+253); SET_HL(wTmpcfc0);
  CYC(b_+253, b_+255); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+255, b_+256); alu_xor(gb, A);
  CYC(b_+256, b_+259); mem_wr(gb, wTmpcfc0 + 0x06, A);
  CALL_C(b_+259, s_loadInteracIdb4_subid4And5_hook, SYM(loadInteracIdb4_subid4And5), b_+262);
  CYC(b_+262, b_+264); A = 0x04;
  CALL_C(b_+264, s_fadeinFromWhiteWithDelay, SYM(fadeinFromWhiteWithDelay), b_+267);
  CYC(b_+267, b_+269); A = 0x1e;
  CYC(b_+269, b_+272); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+272, b_+275);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText16:
  CALL_C(b_+275, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+278);
  if (!(F & FZ)) { RET_TAKEN(b_+278); return; }
  CYC(b_+278, b_+279);
  CYC(b_+279, b_+281); C = 0x16; // TX_5016
  CYC(b_+281, b_+284);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText17:
  CALL_C(b_+284, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+287);
  if (!(F & FZ)) { RET_TAKEN(b_+287); return; }
  CYC(b_+287, b_+288);
  CYC(b_+288, b_+290); C = 0x17; // TX_5017
  CYC(b_+290, b_+293);
  s_showCutscene50xxText_hook(gb); return;

funce:
  CALL_C(b_+293, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+296);
  if (!(F & FZ)) { RET_TAKEN(b_+296); return; }
  CYC(b_+296, b_+297);
  CYC(b_+297, b_+300); SET_HL(wTmpcfc0);
  CYC(b_+300, b_+302); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+302, b_+304); A = 0x3c;
  CYC(b_+304, b_+307); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+307, b_+309); A = 0xbb;
  CALL_C(b_+309, playSound_b00_hook, SYM(playSound_b00), b_+312);
  CYC(b_+312, b_+315);
  s_incTmpcbb3_hook(gb); return;

funcf:
  CYC(b_+315, b_+318); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+318, b_+319); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+319); return; }
  CYC(b_+319, b_+320);
  CALL_C(b_+320, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+323);
  CYC(b_+323, b_+326);
  s_incTmpcbb3_hook(gb); return;

loadRoomOfRitesAndInteractions:
  CYC(b_+326, b_+329); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+329, b_+330); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+330); return; }
  CYC(b_+330, b_+331);
  CALL_C(b_+331, s_bank3CutsceneLoadRoomOfRites_hook, SYM(bank3CutsceneLoadRoomOfRites), b_+334);
  CALL_C(b_+334, s_loadInteracIdb0_hook, SYM(loadInteracIdb0), b_+337);
  CYC(b_+337, b_+339); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+339, playSound_b00_hook, SYM(playSound_b00), b_+342);
  CYC(b_+342, b_+343); alu_xor(gb, A);
  CYC(b_+343, b_+346); mem_wr(gb, wTmpcfc0 + 0x06, A);
  CALL_C(b_+346, s_loadInteracIdb4_subid6And7_hook, SYM(loadInteracIdb4_subid6And7), b_+349);
  CALL_C(b_+349, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+352);
  if (!(F & FZ)) { CYCT(b_+352, b_+354); goto gfx; }
  CYC(b_+352, b_+354);
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
gfx:
  CYC(b_+356, b_+358); A = 0x13;
  CALL_C(b_+358, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+361);
  CALL_C(b_+361, s_fadeinFromBlack, SYM(fadeinFromBlack), b_+364);
  CYC(b_+364, b_+366); A = 0x1e;
  CYC(b_+366, b_+369); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+369, b_+372);
  s_incTmpcbb3_hook(gb); return;

startCutsceneText18:
  CALL_C(b_+372, s_waitUntilFadeIsDone_hook, SYM(waitUntilFadeIsDone), b_+375);
  if (!(F & FZ)) { RET_TAKEN(b_+375); return; }
  CYC(b_+375, b_+376);
  CYC(b_+376, b_+378); C = 0x18; // TX_5018
  CYC(b_+378, b_+381);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText19:
  CALL_C(b_+381, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+384);
  if (!(F & FZ)) { RET_TAKEN(b_+384); return; }
  CYC(b_+384, b_+385);
  CYC(b_+385, b_+387); C = 0x19; // TX_5019
  CYC(b_+387, b_+390);
  s_showCutscene50xxText_hook(gb); return;

startCutsceneText1a:
  CALL_C(b_+390, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+393);
  if (!(F & FZ)) { RET_TAKEN(b_+393); return; }
  CYC(b_+393, b_+394);
  CYC(b_+394, b_+396); C = 0x1a; // TX_501a
  CYC(b_+396, b_+399);
  s_showCutscene50xxText_hook(gb); return;

finish:
  CALL_C(b_+399, s_waitUntilTextInactive_hook, SYM(waitUntilTextInactive), b_+402);
  if (!(F & FZ)) { RET_TAKEN(b_+402); return; }
  CYC(b_+402, b_+403);
  CYC(b_+403, b_+404); alu_xor(gb, A);
  CYC(b_+404, b_+407); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+407, b_+409); A = 0x1f; // GLOBALFLAG_ZELDA_KIDNAPPED_SEEN
  CALL_C(b_+409, s_setGlobalFlag, SYM(setGlobalFlag), b_+412);
  CYC(b_+412, b_+414); A = 0x03;
  CYC(b_+414, b_+417); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+417, b_+420); SET_HL(b_+423); // @warpDest
  CYC(b_+420, b_+423);
  TAIL(setWarpDestVariables);
}

// Two white flashes, then a slow fade to white; Z once the fade is done.
void s_zeldaKidnappedFlashFadeoutToWhite_hook(GB *gb) {
  BASE(zeldaKidnappedFlashFadeoutToWhite);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGenericCutscene_cbb4);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_jump_table(gb));
    if (jt_ == b_+16) { goto func0; }
    else if (jt_ == b_+27) { goto func1; }
    else if (jt_ == b_+43) { goto func3; }
    else if (jt_ == b_+47) { goto func4; }
    else if (jt_ == b_+56) { goto func5; }
    else { HANDOFF(HL); }
  } while (0);

func0:
  CYC(b_+16, b_+18); A = 0x0a;
clearPalettes:
  CYC(b_+18, b_+21); mem_wr(gb, wGenericCutscene_cbb5, A);
  CALL_C(b_+21, s_clearFadingPalettes, SYM(clearFadingPalettes), b_+24);
  CYC(b_+24, b_+27);
  s_incTmpcbb4_hook(gb); return;

func1:
  CYC(b_+27, b_+30); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); A = 0x0a;
fadeOut:
  CYC(b_+34, b_+37); mem_wr(gb, wGenericCutscene_cbb5, A);
  CALL_C(b_+37, s_fastFadeoutToWhite, SYM(fastFadeoutToWhite), b_+40);
  CYC(b_+40, b_+43);
  s_incTmpcbb4_hook(gb); return;

func3:
  CYC(b_+43, b_+45); A = 0x14;
  CYC(b_+45, b_+47);
  goto clearPalettes;

func4:
  CYC(b_+47, b_+50); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); A = 0x1e;
  CYC(b_+54, b_+56);
  goto fadeOut;

func5:
  CYC(b_+56, b_+59);
  s_waitUntilFadeIsDone_hook(gb); return;
}

// Text $50xx (c), a half-second countdown, then the next step.
void s_showCutscene50xxText_hook(GB *gb) {
  BASE(showCutscene50xxText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x50;
  CALL_C(b_+2, s_showText, SYM(showText), b_+5);
  CYC(b_+5, b_+7); A = 0x1e;
  CYC(b_+7, b_+10); mem_wr(gb, wGenericCutscene_cbb5, A);
  s_incTmpcbb3_hook(gb); return;
}

void s_incTmpcbb3_hook(GB *gb) {
  BASE(incTmpcbb3);
  CYC(b_+0, b_+3); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void s_incTmpcbb4_hook(GB *gb) {
  BASE(incTmpcbb4);
  CYC(b_+0, b_+3); SET_HL(wGenericCutscene_cbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// NZ while text is showing; then counts cbb5 down, Z at zero.
void s_waitUntilTextInactive_hook(GB *gb) {
  BASE(waitUntilTextInactive);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+9); return;
}

// NZ while a fade runs; then counts cbb5 down, Z at zero.
void s_waitUntilFadeIsDone_hook(GB *gb) {
  BASE(waitUntilFadeIsDone);
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wGenericCutscene_cbb5);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+9); return;
}

// The Room of Rites (group 5 room $9a) with its palette, scrolled $28 right, scroll mode 0.
void s_bank3CutsceneLoadRoomOfRites_hook(GB *gb) {
  BASE(bank3CutsceneLoadRoomOfRites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); SET_BC(0x059a); // ROOM_ZELDA_IN_FINAL_DUNGEON
  CALL_C(b_+4, s_disableLcdAndLoadRoom_body, SYM(disableLcdAndLoadRoom_body), b_+7);
  CYC(b_+7, b_+9); A = 0xac; // PALH_ac
  CALL_C(b_+9, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+12);
  CYC(b_+12, b_+14); A = 0x28;
  CYC(b_+14, b_+17); mem_wr(gb, wGfxRegs1_SCX, A);
  CYC(b_+17, b_+20); mem_wr(gb, wGfxRegs2_SCX, A);
  CYC(b_+20, b_+22); mem_wr(gb, hCameraX, A);
  CYC(b_+22, b_+24); A = 0x00;
  CYC(b_+24, b_+27); mem_wr(gb, wScrollMode, A);
  CYC(b_+27, b_+29); A = 0x10;
  CYC(b_+29, b_+31); mem_wr(gb, hOamTail, A);
  CYC(b_+31, b_+34);
  TAIL(clearWramBank1);
}

// Two Twinrova flames, subids 4 and 3.
void s_loadInteracIdb0_hook(GB *gb) {
  BASE(loadInteracIdb0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x02;
  for (;;) {
    CALL_C(b_+2, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+5);
    if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+8); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
    CYC(b_+8, b_+9); L = alu_inc8(gb, L);
    CYC(b_+9, b_+11); A = 0x02;
    CYC(b_+11, b_+12); alu_add(gb, B);
    CYC(b_+12, b_+13); B = alu_dec8(gb, B);
    CYC(b_+13, b_+14); mem_wr(gb, HL, A);
    if (!(F & FZ)) { CYCT(b_+14, b_+16); continue; }
    CYC(b_+14, b_+16);
    break;
  }
  RET(b_+16); return;
}

// Interaction group a from @interacGroupTable: id, subid, var03, yh, xh per entry, 0 ends.
void s_loadGroupOfInteractions_hook(GB *gb) {
  BASE(loadGroupOfInteractions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+35); // @interacGroupTable
  CYC(b_+3, b_+4); linked_add_double_index(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+7); C = A;
  for (;;) {
    CYC(b_+7, b_+8); A = mem_rd(gb, BC);
    CYC(b_+8, b_+9); alu_or(gb, A);
    if (F & FZ) { RET_TAKEN(b_+9); return; }
    CYC(b_+9, b_+10);
    CALL_C(b_+10, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+13);
    if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
    CYC(b_+13, b_+14);
    CYC(b_+14, b_+15); A = mem_rd(gb, BC);
    CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+16, b_+17); SET_BC(BC + 1);
    CYC(b_+17, b_+18); A = mem_rd(gb, BC);
    CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+19, b_+20); SET_BC(BC + 1);
    CYC(b_+20, b_+21); A = mem_rd(gb, BC);
    CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+22, b_+23); SET_BC(BC + 1);
    CYC(b_+23, b_+25); L = 0x4b; // Interaction.yh
    CYC(b_+25, b_+26); A = mem_rd(gb, BC);
    CYC(b_+26, b_+27); mem_wr(gb, HL, A);
    CYC(b_+27, b_+28); SET_BC(BC + 1);
    CYC(b_+28, b_+30); L = 0x4d; // Interaction.xh
    CYC(b_+30, b_+31); A = mem_rd(gb, BC);
    CYC(b_+31, b_+32); mem_wr(gb, HL, A);
    CYC(b_+32, b_+33); SET_BC(BC + 1);
    CYC(b_+33, b_+35);
  }
}

// The extra graphics for INTERAC_b4 (Twinrova) with tree gfx 1.
void s_cutscene_func_03_72af_hook(GB *gb) {
  BASE(cutscene_func_03_72af);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+5, b_+7); A = 0xb4;
  CYC(b_+7, b_+10); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  RET(b_+10); return;
}

void s_loadInteracIdb4_subid2And3_hook(GB *gb) {
  BASE(loadInteracIdb4_subid2And3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(loadInteracIdb4) + 0x14); // loadInteracIdb4@subid2
  CALL_C(b_+3, s_loadInteracIdb4_hook, SYM(loadInteracIdb4), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(loadInteracIdb4) + 0x17); // @subid3
  CYC(b_+9, b_+11);
  s_loadInteracIdb4_hook(gb); return;
}

void s_loadInteracIdb4_subid4And5_hook(GB *gb) {
  BASE(loadInteracIdb4_subid4And5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(loadInteracIdb4) + 0x1a); // loadInteracIdb4@subid4
  CALL_C(b_+3, s_loadInteracIdb4_hook, SYM(loadInteracIdb4), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(loadInteracIdb4) + 0x1d); // @subid5
  CYC(b_+9, b_+11);
  s_loadInteracIdb4_hook(gb); return;
}

void s_loadInteracIdb4_subid6And7_hook(GB *gb) {
  BASE(loadInteracIdb4_subid6And7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(loadInteracIdb4) + 0x20); // loadInteracIdb4@subid6
  CALL_C(b_+3, s_loadInteracIdb4_hook, SYM(loadInteracIdb4), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(loadInteracIdb4) + 0x23); // @subid7
  s_loadInteracIdb4_hook(gb); return;
}

// INTERAC_b4 with the subid, yh and xh at bc.
void s_loadInteracIdb4_hook(GB *gb) {
  BASE(loadInteracIdb4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xb4); // INTERAC_b4
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); SET_BC(BC + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); L = 0x4b; // Interaction.yh
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); SET_BC(BC + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0x4d; // Interaction.xh
  CYC(b_+17, b_+18); A = mem_rd(gb, BC);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  RET(b_+19); return;
}
