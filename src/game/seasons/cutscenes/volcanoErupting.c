#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/volcanoErupting.s (CUTSCENE_S_VOLCANO_ERUPTING),
// bank $03. wTmpcbb3 is the stage's sub-state, wTmpcbb4 a countdown, wTmpcbb5/6 the rock
// spawn timers. Stages: 0 load the Subrosia volcano room, 1 shake and rocks, then scroll to
// the next screen, 2 more rocks, 3/4/5 the Temple Remains screens (3 fills it with lava, 5
// warps Link into Subrosia).

static uint16_t volcano_jump_table(GB *gb) {
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

void s_cutsceneHandler_0b_hook(GB *gb) {
  BASE(cutsceneHandler_0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (volcano_jump_table(gb));
    if (jt_ == SYM(cutsceneHandler_0b_stage0) && hook_is(gb, SYM(cutsceneHandler_0b_stage0), s_cutsceneHandler_0b_stage0_hook)) { s_cutsceneHandler_0b_stage0_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage1) && hook_is(gb, SYM(cutsceneHandler_0b_stage1), s_cutsceneHandler_0b_stage1_hook)) { s_cutsceneHandler_0b_stage1_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage2) && hook_is(gb, SYM(cutsceneHandler_0b_stage2), s_cutsceneHandler_0b_stage2_hook)) { s_cutsceneHandler_0b_stage2_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3) && hook_is(gb, SYM(cutsceneHandler_0b_stage3), s_cutsceneHandler_0b_stage3_hook)) { s_cutsceneHandler_0b_stage3_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage4) && hook_is(gb, SYM(cutsceneHandler_0b_stage4), s_cutsceneHandler_0b_stage4_hook)) { s_cutsceneHandler_0b_stage4_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage5) && hook_is(gb, SYM(cutsceneHandler_0b_stage5), s_cutsceneHandler_0b_stage5_hook)) { s_cutsceneHandler_0b_stage5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Once the fade is done: the Subrosia volcano room (group 1 room $03) in winter, the disaster
// music, and a two-second countdown.
void s_cutsceneHandler_0b_stage0_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, disableLcd_hook, SYM(disableLcd), b_+8);
  CALL_C(b_+8, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+11);
  CYC(b_+11, b_+13); A = 0x03; // SEASON_WINTER
  CYC(b_+13, b_+16); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+16, b_+19); SET_BC(0x0103); // ROOM_SEASONS_103
  CALL_C(b_+19, s_seasonsFunc_03_6de4_hook, SYM(seasonsFunc_03_6de4), b_+22);
  CYC(b_+22, b_+24); A = 0x78;
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+27, b_+29); A = 0x01;
  CYC(b_+29, b_+32); mem_wr(gb, wCutsceneState, A);
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+36, b_+38); A = 0x21; // MUS_DISASTER
  CYC(b_+38, b_+41); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+41, playSound_b00_hook, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+46); A = 0xb0; // SND_OPENING
  CALL_C(b_+46, playSound_b00_hook, SYM(playSound_b00), b_+49);
  CYC(b_+49, b_+51); A = 0x01;
  CYC(b_+51, b_+54); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+54, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+57);
  CALL_C(b_+57, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+60);
  CYC(b_+60, b_+62); A = 0x02;
  CYC(b_+62, b_+65);
  TAIL(loadGfxRegisterStateIndex);
}

// The volcano smokes and throws rocks, then the screen scrolls up to the next room, whose
// objects are set up, and the stage advances.
void s_cutsceneHandler_0b_stage1_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_03_6df8_hook, SYM(seasonsFunc_03_6df8), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (volcano_jump_table(gb));
    if (jt_ == b_+15) { goto state0; }
    else if (jt_ == b_+41) { goto state1; }
    else if (jt_ == b_+63) { goto state2; }
    else if (jt_ == b_+103) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+15, b_+18); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CALL_C(b_+20, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); A = 0xb0; // SND_OPENING
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CYC(b_+29, b_+32); SET_HL(wTmpcbb4);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x96);
  CYC(b_+34, b_+35); SET_HL(HL + 1);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x01);
  CYC(b_+37, b_+40); SET_HL(wTmpcbb3);
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state1:
  CYC(b_+41, b_+44); SET_BC(0x1478);
  CYC(b_+44, b_+47); SET_HL(wTmpcbb5);
  CALL_C(b_+47, s_seasonsFunc_03_6db1_hook, SYM(seasonsFunc_03_6db1), b_+50);
  CALL_C(b_+50, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); A = 0x81;
  CYC(b_+56, b_+59); mem_wr(gb, wScreenTransitionDirection, A);
  CYC(b_+59, b_+62); SET_HL(wTmpcbb3);
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state2:
  CYC(b_+63, b_+66); A = mem_rd(gb, wScrollMode);
  CYC(b_+66, b_+68); alu_and(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0x04;
  CYC(b_+71, b_+74); mem_wr(gb, wActiveRoom, A);
  CYC(b_+74, b_+77); SET_HL(SYM(setObjectsEnabledTo2));
  CYC(b_+77, b_+79); E = 0x01;
  CALL_C(b_+79, interBankCall_hook, SYM(interBankCall), b_+82);
  CALL_C(b_+82, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+85);
  CALL_C(b_+85, loadTilesetData_hook, SYM(loadTilesetData), b_+88);
  CALL_C(b_+88, loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+91);
  CALL_C(b_+91, s_generateVramTilesWithRoomChanges, SYM(generateVramTilesWithRoomChanges), b_+94);
  CYC(b_+94, b_+96); A = 0x08;
  CYC(b_+96, b_+99); mem_wr(gb, wScrollMode, A);
  CYC(b_+99, b_+102); SET_HL(wTmpcbb3);
  CYC(b_+102, b_+103); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state3:
  CYC(b_+103, b_+106); A = mem_rd(gb, wScrollMode);
  CYC(b_+106, b_+108); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+112); SET_HL(SYM(clearObjectsWithEnabled2));
  CYC(b_+112, b_+114); E = 0x01;
  CALL_C(b_+114, interBankCall_hook, SYM(interBankCall), b_+117);
  CYC(b_+117, b_+120); SET_HL(wTmpcbb4);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x96);
  CYC(b_+122, b_+123); SET_HL(HL + 1);
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x01);
  CYC(b_+125, b_+126); SET_HL(HL + 1);
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x01);
  CYC(b_+128, b_+130); A = 0xb0; // SND_OPENING
  CALL_C(b_+130, playSound_b00_hook, SYM(playSound_b00), b_+133);
  TAIL_S(seasonsFunc_03_6c5f);
}

// Next stage, sub-state 0.
void s_seasonsFunc_03_6c5f_hook(GB *gb) {
  BASE(seasonsFunc_03_6c5f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); SET_HL(wTmpcbb3);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  RET(b_+9); return;
}

// Rocks from two spots until the countdown ends, then a fast fade to white.
void s_cutsceneHandler_0b_stage2_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_03_6df8_hook, SYM(seasonsFunc_03_6df8), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x1430);
  CYC(b_+6, b_+9); SET_HL(wTmpcbb5);
  CALL_C(b_+9, s_seasonsFunc_03_6db1_hook, SYM(seasonsFunc_03_6db1), b_+12);
  CYC(b_+12, b_+15); SET_BC(0x1488);
  CYC(b_+15, b_+18); SET_HL(wTmpcbb6);
  CALL_C(b_+18, s_seasonsFunc_03_6db1_hook, SYM(seasonsFunc_03_6db1), b_+21);
  CYC(b_+21, b_+24); SET_HL(wTmpcbb4);
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CALL_C(b_+26, s_seasonsFunc_03_6c5f_hook, SYM(seasonsFunc_03_6c5f), b_+29);
  CYC(b_+29, b_+32);
  TAIL(fastFadeoutToWhite);
}

// cutsceneHandler_0b_stage3@state4 (shared by stages 3 and 4): rocks until the countdown ends,
// then the next stage and a fade to white.
static void volcano_state4(GB *gb) {
  BASE(cutsceneHandler_0b_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+117, b_+120); SET_HL(wTmpcbb5);
  CALL_C(b_+120, s_seasonsFunc_03_6dcb_hook, SYM(seasonsFunc_03_6dcb), b_+123);
  CALL_C(b_+123, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+126);
  if (!(F & FZ)) { RET_TAKEN(b_+126); return; }
  CYC(b_+126, b_+127);
  CALL_C(b_+127, s_seasonsFunc_03_6c5f_hook, SYM(seasonsFunc_03_6c5f), b_+130);
  CYC(b_+130, b_+132); A = 0x02;
  CYC(b_+132, b_+135);
  TAIL(fadeoutToWhiteWithDelay);
}

// cutsceneHandler_0b_stage3@state3: once faded, the Temple Remains with lava, the layout
// patch object (INTERAC_MISCELLANEOUS_2 subid $0e) and a new countdown.
static void volcano_state3(GB *gb) {
  BASE(cutsceneHandler_0b_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+87, b_+90); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+90, b_+91); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CALL_C(b_+92, s_seasonsFunc_03_6d8b_hook, SYM(seasonsFunc_03_6d8b), b_+95);
  CALL_C(b_+95, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto next; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0xdc); // INTERAC_MISCELLANEOUS_2
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x0e);
next:
  CYC(b_+105, b_+108); SET_HL(wTmpcbb3);
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+109, b_+112); SET_HL(wTmpcbb4);
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x78);
  CALL_C(b_+114, s_seasonsFunc_03_6df8_hook, SYM(seasonsFunc_03_6df8), b_+117);
  volcano_state4(gb); return;
}

// cutsceneHandler_0b_stage3@state2: rocks until the countdown ends, then a fade to white.
static void volcano_state2(GB *gb) {
  BASE(cutsceneHandler_0b_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+65, b_+68); SET_HL(wTmpcbb5);
  CALL_C(b_+68, s_seasonsFunc_03_6dcb_hook, SYM(seasonsFunc_03_6dcb), b_+71);
  CALL_C(b_+71, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+74);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; }
  CYC(b_+74, b_+75);
  CALL_C(b_+75, s_seasonsFunc_03_6df8_hook, SYM(seasonsFunc_03_6df8), b_+78);
  CYC(b_+78, b_+81); SET_HL(wTmpcbb3);
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+82, b_+84); A = 0x02;
  CALL_C(b_+84, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+87);
  volcano_state3(gb); return;
}

// cutsceneHandler_0b_stage3@state1: after the countdown, shaking and a two-second countdown.
static void volcano_state1(GB *gb) {
  BASE(cutsceneHandler_0b_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+46, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+49);
  if (!(F & FZ)) { RET_TAKEN(b_+49); return; }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, s_seasonsFunc_03_6df8_hook, SYM(seasonsFunc_03_6df8), b_+53);
  CYC(b_+53, b_+56); SET_HL(wTmpcbb4);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x78);
  CYC(b_+58, b_+59); SET_HL(HL + 1);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x01);
  CYC(b_+61, b_+64); SET_HL(wTmpcbb3);
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  volcano_state2(gb); return;
}

// Stages 4 and 5 dispatch into stage 3's states 1..4 by address.
// The Temple Remains screen: loaded (room $15), then the eruption and lava.
void s_cutsceneHandler_0b_stage3_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (volcano_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state1)) { volcano_state1(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state2)) { volcano_state2(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state3)) { volcano_state3(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state4)) { volcano_state4(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+14, b_+17); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+22);
  CALL_C(b_+22, disableLcd_hook, SYM(disableLcd), b_+25);
  CALL_C(b_+25, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+28);
  CYC(b_+28, b_+31); SET_BC(0x0015); // ROOM_SEASONS_015
  CALL_C(b_+31, s_seasonsFunc_03_6de4_hook, SYM(seasonsFunc_03_6de4), b_+34);
  CYC(b_+34, b_+36); A = 0x1e;
  CYC(b_+36, b_+39); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+39, b_+42); SET_HL(wTmpcbb3);
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+46);
  TAIL_S(seasonsFunc_03_6d9f);
}

// Stages 4 and 5 open another screen (room bc) the same way, the lava flag cleared.
static void volcano_open_screen(GB *gb, uint16_t b_, uint16_t room) {
  BANKOF(cutsceneHandler_0b_stage4);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+14, b_+17); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+22);
  CALL_C(b_+22, disableLcd_hook, SYM(disableLcd), b_+25);
  CALL_C(b_+25, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+28);
  CYC(b_+28, b_+30); A = 0x15; // GLOBALFLAG_TEMPLE_REMAINS_FILLED_WITH_LAVA
  CALL_C(b_+30, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+33);
  CYC(b_+33, b_+36); SET_BC(room);
  CALL_C(b_+36, s_seasonsFunc_03_6de4_hook, SYM(seasonsFunc_03_6de4), b_+39);
  CYC(b_+39, b_+41); A = 0x1e;
  CYC(b_+41, b_+44); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+44, b_+47); SET_HL(wTmpcbb3);
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+48, b_+51);
  TAIL_S(seasonsFunc_03_6d9f);
}

void s_cutsceneHandler_0b_stage4_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (volcano_jump_table(gb));
    if (jt_ == b_+14) { volcano_open_screen(gb, b_, 0x0027); return; } // ROOM_SEASONS_027
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state1)) { volcano_state1(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state2)) { volcano_state2(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state3)) { volcano_state3(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state4)) { volcano_state4(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// The last screen (room $17); when its countdown ends Link warps into Subrosia.
void s_cutsceneHandler_0b_stage5_hook(GB *gb) {
  BASE(cutsceneHandler_0b_stage5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (volcano_jump_table(gb));
    if (jt_ == b_+14) { volcano_open_screen(gb, b_, 0x0017); return; } // ROOM_SEASONS_017
    else if (jt_ == b_+51) { goto state4; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state1)) { volcano_state1(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state2)) { volcano_state2(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state3)) { volcano_state3(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0b_stage3__state4)) { volcano_state4(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state4:
  CYC(b_+51, b_+54); SET_HL(wTmpcbb5);
  CALL_C(b_+54, s_seasonsFunc_03_6dcb_hook, SYM(seasonsFunc_03_6dcb), b_+57);
  CALL_C(b_+57, s_seasonsFunc_03_6ddf_hook, SYM(seasonsFunc_03_6ddf), b_+60);
  if (!(F & FZ)) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+64); SET_HL(b_+67); // @warpDestVariables
  CYC(b_+64, b_+67);
  TAIL(setWarpDestVariables);
}

// The Temple Remains filled with lava: tileset and graphics reloaded, then the fade in.
void s_seasonsFunc_03_6d8b_hook(GB *gb) {
  BASE(seasonsFunc_03_6d8b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+6);
  CYC(b_+6, b_+8); A = 0x15; // GLOBALFLAG_TEMPLE_REMAINS_FILLED_WITH_LAVA
  CALL_C(b_+8, setGlobalFlag_hook, SYM(setGlobalFlag), b_+11);
  CALL_C(b_+11, loadTilesetData_hook, SYM(loadTilesetData), b_+14);
  CALL_C(b_+14, s_loadTilesetGraphics, SYM(loadTilesetGraphics), b_+17);
  CALL_C(b_+17, s_func_131f, SYM(func_131f), b_+20);
  TAIL_S(seasonsFunc_03_6d9f);
}

// Scroll mode 1, fade in from white, common graphics and the gfx register state.
void s_seasonsFunc_03_6d9f_hook(GB *gb) {
  BASE(seasonsFunc_03_6d9f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wScrollMode, A);
  CYC(b_+5, b_+7); A = 0x02;
  CALL_C(b_+7, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+10);
  CALL_C(b_+10, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+13);
  CYC(b_+13, b_+15); A = 0x02;
  CYC(b_+15, b_+18);
  TAIL(loadGfxRegisterStateIndex);
}

// Counts the timer at hl down; at zero, re-arms it randomly ($08..$17) and throws a volcano
// rock (subid 1) from (b,c).
void s_seasonsFunc_03_6db1_hook(GB *gb) {
  BASE(seasonsFunc_03_6db1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+1); return; }
  CYC(b_+1, b_+2);
  CALL_C(b_+2, getRandomNumber_hook, SYM(getRandomNumber), b_+5);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CALL_C(b_+10, getFreePartSlot_hook, SYM(getFreePartSlot), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x11); // PART_VOLCANO_ROCK
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x01);
  CYC(b_+19, b_+21); L = 0xcb; // Part.yh
  CYC(b_+21, b_+22); mem_wr(gb, HL, B);
  CYC(b_+22, b_+24); L = 0xcd; // Part.xh
  CYC(b_+24, b_+25); mem_wr(gb, HL, C);
  RET(b_+25); return;
}

// The same with a falling rock (subid 2) from its default position.
void s_seasonsFunc_03_6dcb_hook(GB *gb) {
  BASE(seasonsFunc_03_6dcb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+1); return; }
  CYC(b_+1, b_+2);
  CALL_C(b_+2, getRandomNumber_hook, SYM(getRandomNumber), b_+5);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CALL_C(b_+10, getFreePartSlot_hook, SYM(getFreePartSlot), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x11); // PART_VOLCANO_ROCK
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x02);
  RET(b_+19); return;
}

// Counts wTmpcbb4 down; Z at zero.
void s_seasonsFunc_03_6ddf_hook(GB *gb) {
  BASE(seasonsFunc_03_6ddf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// Loads room bc (group, room): music, room pack, tileset data and graphics.
void s_seasonsFunc_03_6de4_hook(GB *gb) {
  BASE(seasonsFunc_03_6de4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+4); mem_wr(gb, wActiveGroup, A);
  CYC(b_+4, b_+5); A = C;
  CYC(b_+5, b_+8); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+8, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+11);
  CALL_C(b_+11, loadTilesetData_hook, SYM(loadTilesetData), b_+14);
  CALL_C(b_+14, s_loadTilesetGraphics, SYM(loadTilesetGraphics), b_+17);
  CYC(b_+17, b_+20);
  TAIL_SG(func_131f);
}

// Endless screen shake.
void s_seasonsFunc_03_6df8_hook(GB *gb) {
  BASE(seasonsFunc_03_6df8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5);
  TAIL(setScreenShakeCounter);
}
