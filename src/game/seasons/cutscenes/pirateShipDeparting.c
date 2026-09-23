#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/pirateShipDeparting.s (CUTSCENE_S_PIRATES_DEPART),
// bank $03. wTmpcbb3 is the stage's sub-state, wTmpcbb4 a countdown, wTmpcbb7 the next ship
// frame, wTmpcbb8/wTmpcbbf the ship's window line, wTmpcbbd/wTmpcbbe the wave animation.
// Stages: 0 setup, 1 digging out of Subrosia, 2 inside the ship, 3 over Samasa Desert, 4 back
// inside, 5 arriving at the west coast.

static uint16_t pirates_jump_table(GB *gb) {
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

static void pirates_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void pirates_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void s_cutsceneHandler_0c_hook(GB *gb) {
  BASE(cutsceneHandler_0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == SYM(cutsceneHandler_0c_stage0) && hook_is(gb, SYM(cutsceneHandler_0c_stage0), s_cutsceneHandler_0c_stage0_hook)) { s_cutsceneHandler_0c_stage0_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0c_stage1) && hook_is(gb, SYM(cutsceneHandler_0c_stage1), s_cutsceneHandler_0c_stage1_hook)) { s_cutsceneHandler_0c_stage1_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0c_stage2) && hook_is(gb, SYM(cutsceneHandler_0c_stage2), s_cutsceneHandler_0c_stage2_hook)) { s_cutsceneHandler_0c_stage2_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0c_stage3) && hook_is(gb, SYM(cutsceneHandler_0c_stage3), s_cutsceneHandler_0c_stage3_hook)) { s_cutsceneHandler_0c_stage3_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0c_stage4) && hook_is(gb, SYM(cutsceneHandler_0c_stage4), s_cutsceneHandler_0c_stage4_hook)) { s_cutsceneHandler_0c_stage4_hook(gb); return; }
    else if (jt_ == SYM(cutsceneHandler_0c_stage5) && hook_is(gb, SYM(cutsceneHandler_0c_stage5), s_cutsceneHandler_0c_stage5_hook)) { s_cutsceneHandler_0c_stage5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Clears the cutscene variables and bank 1, keeps the current scroll line, disables the menu.
void s_cutsceneHandler_0c_stage0_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x10;
  CYC(b_+2, b_+5); SET_HL(wTmpcbb3);
  CALL_C(b_+5, s_clearMemory, SYM(clearMemory), b_+8);
  CALL_C(b_+8, s_clearWramBank1, SYM(clearWramBank1), b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+18); mem_wr(gb, wScrollMode, A);
  CYC(b_+18, b_+21); A = mem_rd(gb, wGfxRegs2_SCY);
  CYC(b_+21, b_+24); mem_wr(gb, wTmpcbba, A);
  CYC(b_+24, b_+26); A = 0x80;
  CYC(b_+26, b_+29); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+29, b_+31); A = 0x01;
  CYC(b_+31, b_+34); mem_wr(gb, wCutsceneState, A);
  RET(b_+34); return;
}

// cutsceneHandler_0c_stage1@state0Func0: sand (tile $0f) over the ship's hole.
static void pirates_sand_tiles(GB *gb) {
  BASE(cutsceneHandler_0c_stage1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+58, b_+61); SET_HL(b_+76); // @subrosianSandReplacementPositions
  CYC(b_+61, b_+63); D = 0x0f;
  for (;;) {
    CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+64, b_+65); C = A;
    CYC(b_+65, b_+67); A = 0x0f;
    CYC(b_+67, b_+68); push_effect(gb, HL);
    CALL_C(b_+68, s_setTile, SYM(setTile), b_+71);
    CYC(b_+71, b_+72); SET_HL(pop_effect(gb));
    CYC(b_+72, b_+73); D = alu_dec8(gb, D);
    if (!(F & FZ)) { CYCT(b_+73, b_+75); continue; }
    CYC(b_+73, b_+75);
    break;
  }
  RET(b_+75); return;
}

// Every 8 frames: the ship digs up through the Subrosian sand, with text, sand puffs and
// music, then fades to white and moves on to stage 2.
void s_cutsceneHandler_0c_stage1_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_03_6b6c_hook, SYM(seasonsFunc_03_6b6c), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == b_+25) { goto state0; }
    else if (jt_ == b_+91) { goto state1; }
    else if (jt_ == b_+108) { goto state2; }
    else if (jt_ == b_+125) { goto state3; }
    else if (jt_ == b_+144) { goto state4; }
    else if (jt_ == b_+160) { goto state5; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+25, s_incCbb3_hook, SYM(incCbb3), b_+28);
  CYC(b_+28, b_+30); A = 0x08;
  CYC(b_+30, b_+33); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+33, b_+35); A = 0x04;
  CYC(b_+35, b_+38); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+38, b_+40); A = 0x51; // GFXH_PIRATE_SHIP_LEAVING_SUBROSIA_LAYOUT
  CALL_C(b_+40, s_loadGfxHeader, SYM(loadGfxHeader), b_+43);
  CYC(b_+43, b_+45); A = 0x54; // GFXH_PIRATE_SHIP_MOVING_EXTRA_TILES
  CALL_C(b_+45, s_loadGfxHeader, SYM(loadGfxHeader), b_+48);
  CYC(b_+48, b_+50); A = 0x04;
  CYC(b_+50, b_+52); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CALL_C(b_+52, s_seasonsFunc_03_681a_hook, SYM(seasonsFunc_03_681a), b_+55);
  CYC(b_+55, b_+58);
  s_seasonsFunc_03_67f8_hook(gb); return;

state1:
  CYC(b_+91, b_+94); SET_HL(wTmpcbb4);
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+99); SET_BC(0x4e00); // TX_4e00
  CALL_C(b_+99, s_showText, SYM(showText), b_+102);
  CALL_L(b_+102, pirates_sand_tiles, b_+105);
  CYC(b_+105, b_+108);
  s_incCbb3_hook(gb); return;

state2:
  CALL_C(b_+108, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+111);
  CYC(b_+111, b_+114); SET_HL(SYM(objectData_sandPuffsFromShipDigging));
  CALL_C(b_+114, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+117);
  CYC(b_+117, b_+119); A = 0x20; // MUS_TRIUMPHANT
  CALL_C(b_+119, playSound_b00_hook, SYM(playSound_b00), b_+122);
  CYC(b_+122, b_+125);
  s_incCbb3_hook(gb); return;

state3:
  CALL_C(b_+125, s_incCbbfAndCbb8_hook, SYM(incCbbfAndCbb8), b_+128);
  CYC(b_+128, b_+129); A = mem_rd(gb, HL);
  CYC(b_+129, b_+131); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+131, b_+133); goto next; }
  CYC(b_+131, b_+133);
  CALL_C(b_+133, s_seasonsFunc_03_681a_hook, SYM(seasonsFunc_03_681a), b_+136);
  if (!(F & FZ)) { CYCT(b_+136, b_+138); goto next; }
  CYC(b_+136, b_+138);
  CALL_C(b_+138, s_incCbb3_hook, SYM(incCbb3), b_+141);
next:
  CYC(b_+141, b_+144);
  s_seasonsFunc_03_67f8_hook(gb); return;

state4:
  CALL_C(b_+144, s_incCbbfAndCbb8_hook, SYM(incCbbfAndCbb8), b_+147);
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);
  CYC(b_+148, b_+150); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(b_+150, b_+152); s_seasonsFunc_03_67f8_hook(gb); return; }
  CYC(b_+150, b_+152);
  CALL_C(b_+152, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+155);
  CALL_C(b_+155, s_incCbb3_hook, SYM(incCbb3), b_+158);
  CYC(b_+158, b_+160);
  s_seasonsFunc_03_67f8_hook(gb); return;

state5:
  CALL_C(b_+160, s_incCbbfAndCbb8_hook, SYM(incCbbfAndCbb8), b_+163);
  CYC(b_+163, b_+166); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+167, b_+169); s_seasonsFunc_03_67f8_hook(gb); return; }
  CYC(b_+167, b_+169);
  CYC(b_+169, b_+171); A = 0xc7;
  CYC(b_+171, b_+174); mem_wr(gb, wGfxRegs1_WINY, A);
  CYC(b_+174, b_+177); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(b_+177, b_+179); A = 0x03;
  CYC(b_+179, b_+181); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+181, b_+183); A = 0x02;
  s_seasonsFunc_03_67e9_hook(gb); return;
}

// Cutscene stage a, sub-state 0, and wTmpcfc0 cleared.
void s_seasonsFunc_03_67e9_hook(GB *gb) {
  BASE(seasonsFunc_03_67e9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wCutsceneState, A);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+7); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+7, b_+9); B = 0x10;
  CYC(b_+9, b_+12); SET_HL(wTmpcbb3);
  CYC(b_+12, b_+15);
  TAIL(clearMemory);
}

// The ship in the window from line wTmpcbb8, the LCD interrupt at line $40, then the waves.
void s_seasonsFunc_03_67f8_hook(GB *gb) {
  BASE(seasonsFunc_03_67f8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40;
  CYC(b_+2, b_+5); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+5, b_+7); A = 0x47;
  CYC(b_+7, b_+10); mem_wr(gb, wGfxRegs2_WINX, A);
  CYC(b_+10, b_+12); A = 0xa5;
  CYC(b_+12, b_+15); mem_wr(gb, wGfxRegs1_WINX, A);
  CYC(b_+15, b_+18); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+18, b_+21); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(b_+21, b_+24); mem_wr(gb, wGfxRegs1_WINY, A);
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcbbc, A);
  CYC(b_+27, b_+29);
  s_seasonsFunc_03_684c_hook(gb); return;
}

void s_incCbb3_hook(GB *gb) {
  BASE(incCbb3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// The next frame of the ship digging out (w2TmpGfxBuffer row from seasonsTable_03_6844, $ff
// ends it with Z): its tiles and attributes queued to the window map.
void s_seasonsFunc_03_681a_hook(GB *gb) {
  BASE(seasonsFunc_03_681a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+3, b_+6); SET_HL(SYM(seasonsTable_03_6844));
  CYC(b_+6, b_+7); pirates_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+14); H = 0xd0;
  CYC(b_+14, b_+15); push_effect(gb, HL);
  CYC(b_+15, b_+18); SET_DE(0x9c00);
  CYC(b_+18, b_+21); SET_BC(0x0f02);
  CALL_C(b_+21, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+24);
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+27); H = H | 0x04;
  CYC(b_+27, b_+29); E = 0x01;
  CALL_C(b_+29, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+32);
  CYC(b_+32, b_+34); A = 0x08;
  CYC(b_+34, b_+37); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+37, b_+40); SET_HL(wTmpcbb7);
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+41); return;
}

// Wave animation frame (wTmpcbb8 & 7) from the bank 2 ship tiles into VRAM $94e1.
void s_seasonsFunc_03_684c_hook(GB *gb) {
  BASE(seasonsFunc_03_684c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+7, b_+9); alu_and(gb, 0x07);
  CYC(b_+9, b_+12); SET_HL(w2WaveScrollValues);
  CYC(b_+12, b_+13); pirates_add_double_index(gb, b_+13);
  CYC(b_+13, b_+16); SET_DE(w2Filler6 + 0x50);
  CYC(b_+16, b_+18); B = 0x10;
  CALL_C(b_+18, s_copyMemory, SYM(copyMemory), b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+24, b_+26); alu_and(gb, 0x07);
  CYC(b_+26, b_+29); SET_HL(w2WaveScrollValues + 0x20);
  CYC(b_+29, b_+30); pirates_add_double_index(gb, b_+30);
  CYC(b_+30, b_+33); SET_DE(w2Filler6 + 0x60);
  CYC(b_+33, b_+35); B = 0x10;
  CALL_C(b_+35, s_copyMemory, SYM(copyMemory), b_+38);
  CYC(b_+38, b_+40); A = 0x00;
  CYC(b_+40, b_+42); mem_wr(gb, IO_SVBK, A);
  CYC(b_+42, b_+45); SET_HL(w2Filler6 + 0x50);
  CYC(b_+45, b_+48); SET_DE(0x94e1);
  CYC(b_+48, b_+51); SET_BC(0x0102);
  CYC(b_+51, b_+54);
  TAIL(queueDmaTransfer);
}

// cutsceneHandler_0c_stage3@state0Func1: scroll mode 1, fade in, common graphics, then gfx
// register state a.
static void pirates_fade_in(GB *gb) {
  BASE(cutsceneHandler_0c_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+95, b_+96); push_effect(gb, AF);
  CYC(b_+96, b_+98); A = 0x01;
  CYC(b_+98, b_+101); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+101, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+104);
  CALL_C(b_+104, s_loadCommonGraphics, SYM(loadCommonGraphics), b_+107);
  CYC(b_+107, b_+108); SET_AF(pop_effect(gb));
  CYC(b_+108, b_+111);
  TAIL(loadGfxRegisterStateIndex);
}

// cutsceneHandler_0c_stage3@loadNewRoom: room bc (group, room) with its music, tileset and
// graphics.
static void pirates_load_room(GB *gb) {
  BASE(cutsceneHandler_0c_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+75, b_+76); A = B;
  CYC(b_+76, b_+79); mem_wr(gb, wActiveGroup, A);
  CYC(b_+79, b_+80); A = C;
  CYC(b_+80, b_+83); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+83, s_loadScreenMusicAndSetRoomPack, SYM(loadScreenMusicAndSetRoomPack), b_+86);
  CALL_C(b_+86, s_loadTilesetData, SYM(loadTilesetData), b_+89);
  CALL_C(b_+89, s_loadTilesetGraphics, SYM(loadTilesetGraphics), b_+92);
  CYC(b_+92, b_+95);
  TAIL_SG(func_131f);
}

// cutsceneHandler_0c_stage3@state0Func2: the ship's shadow tiles over the desert
// (position/tile pairs, $ff ends).
static void pirates_desert_tiles(GB *gb) {
  BASE(cutsceneHandler_0c_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+111, b_+114); SET_HL(b_+127); // @state0Table0
  for (;;) {
    CYC(b_+114, b_+115); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+115, b_+117); alu_cp(gb, 0xff);
    if (F & FZ) { RET_TAKEN(b_+117); return; }
    CYC(b_+117, b_+118);
    CYC(b_+118, b_+119); C = A;
    CYC(b_+119, b_+120); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+120, b_+121); push_effect(gb, HL);
    CALL_C(b_+121, s_setTile, SYM(setTile), b_+124);
    CYC(b_+124, b_+125); SET_HL(pop_effect(gb));
    CYC(b_+125, b_+127);
  }
}

// cutsceneHandler_0c_stage2@seasonsFunc_03_688c: the ship's hold (room $5d4) with Link at
// ($58,$70) facing down, the ship rocking, then the fade in with gfx register state 2.
static void pirates_enter_ship(GB *gb) {
  BASE(cutsceneHandler_0c_stage2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+10, s_disableLcd, SYM(disableLcd), b_+13);
  CALL_C(b_+13, s_clearScreenVariablesAndWramBank1, SYM(clearScreenVariablesAndWramBank1), b_+16);
  CALL_C(b_+16, s_incCbb3_hook, SYM(incCbb3), b_+19);
  CYC(b_+19, b_+22); SET_BC(0x05d4); // ROOM_SEASONS_5d4
  CALL_L(b_+22, pirates_load_room, b_+25);
  CYC(b_+25, b_+28); SET_HL(w1Link_enabled);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x03);
  CYC(b_+30, b_+32); L = 0x0b; // w1Link.yh
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x58);
  CYC(b_+34, b_+36); L = 0x0d; // w1Link.xh
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x70);
  CYC(b_+38, b_+40); L = 0x08; // w1Link.direction
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x02);
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+46); mem_wr(gb, wLinkForceState, A);
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+51); mem_wr(gb, wScreenShakeMagnitude, A);
  CALL_C(b_+51, s_resetCamera, SYM(resetCamera), b_+54);
  CYC(b_+54, b_+56); A = 0x02;
  CYC(b_+56, b_+59);
  pirates_fade_in(gb); return;
}

// Inside the ship leaving Subrosia; state 2 (set by an object) moves on to stage 3.
void s_cutsceneHandler_0c_stage2_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == b_+59) { goto state0; }
    else if (jt_ == b_+73) { goto state1; }
    else if (jt_ == b_+74) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+59, b_+62); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; }
  CYC(b_+63, b_+64);
  CALL_L(b_+64, pirates_enter_ship, b_+67);
  CYC(b_+67, b_+70); SET_HL(SYM(objectData_insidePirateShipLeavingSubrosia));
  CYC(b_+70, b_+73);
  TAIL(parseGivenObjectData_b00);

state1:
  RET(b_+73); return;

state2:
  CYC(b_+74, b_+76); A = 0x03;
  CALL_C(b_+76, s_seasonsFunc_03_67e9_hook, SYM(seasonsFunc_03_67e9), b_+79);
  CYC(b_+79, b_+82);
  TAIL(fadeoutToWhite);
}

// Over Samasa Desert (room $0fe): text, then the ship sails up out of the window and the
// screen fades to white into stage 4.
void s_cutsceneHandler_0c_stage3_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+160) { goto state1; }
    else if (jt_ == b_+179) { goto state2; }
    else if (jt_ == b_+190) { goto state3; }
    else if (jt_ == b_+217) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+14, b_+17); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, s_disableLcd, SYM(disableLcd), b_+22);
  CALL_C(b_+22, s_clearScreenVariablesAndWramBank1, SYM(clearScreenVariablesAndWramBank1), b_+25);
  CALL_C(b_+25, s_incCbb3_hook, SYM(incCbb3), b_+28);
  CYC(b_+28, b_+30); A = 0x40;
  CYC(b_+30, b_+33); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbbf, A);
  CYC(b_+36, b_+38); A = 0x1e;
  CYC(b_+38, b_+41); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+46); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+46, b_+49); SET_BC(0x00fe); // ROOM_SEASONS_0fe
  CALL_L(b_+49, pirates_load_room, b_+52);
  CALL_L(b_+52, pirates_desert_tiles, b_+55);
  CYC(b_+55, b_+57); E = 0x0c;
  CALL_C(b_+57, s_loadObjectGfxHeaderToSlot4, SYM(loadObjectGfxHeaderToSlot4), b_+60);
  CYC(b_+60, b_+62); A = 0x52; // GFXH_PIRATE_SHIP_LEAVING_DESERT_LAYOUT
  CALL_C(b_+62, s_loadGfxHeader, SYM(loadGfxHeader), b_+65);
  CYC(b_+65, b_+68); SET_HL(SYM(objectData_leavingSamasaDesert));
  CALL_C(b_+68, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+71);
  CYC(b_+71, b_+73); A = 0x11;
  CYC(b_+73, b_+75);
  pirates_fade_in(gb); return;

state1:
  CYC(b_+160, b_+163); SET_HL(wTmpcbb4);
  CYC(b_+163, b_+164); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+164); return; }
  CYC(b_+164, b_+165);
  CALL_C(b_+165, s_incCbb3_hook, SYM(incCbb3), b_+168);
  CYC(b_+168, b_+169); alu_xor(gb, A);
  CYC(b_+169, b_+171); mem_wr(gb, hCameraY, A);
  CYC(b_+171, b_+173); mem_wr(gb, hCameraX, A);
  CYC(b_+173, b_+176); SET_BC(0x4e09); // TX_4e09
  CYC(b_+176, b_+179);
  TAIL(showText);

state2:
  CALL_C(b_+179, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+182);
  CYC(b_+182, b_+184); A = 0xff;
  CYC(b_+184, b_+187); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+187, b_+190);
  s_incCbb3_hook(gb); return;

state3:
  CYC(b_+190, b_+193); A = mem_rd(gb, wFrameCounter);
  CYC(b_+193, b_+195); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+195); return; }
  CYC(b_+195, b_+196);
  CALL_C(b_+196, s_incCbbfAndCbb8_hook, SYM(incCbbfAndCbb8), b_+199);
  CYC(b_+199, b_+200); A = mem_rd(gb, HL);
  CYC(b_+200, b_+202); alu_cp(gb, 0x70);
  if (F & FC) { CYCT(b_+202, b_+204); s_seasonsFunc_03_69d1_hook(gb); return; }
  CYC(b_+202, b_+204);
  CALL_C(b_+204, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+207);
  CYC(b_+207, b_+209); A = 0xfb;
  CALL_C(b_+209, playSound_b00_hook, SYM(playSound_b00), b_+212);
  CALL_C(b_+212, s_incCbb3_hook, SYM(incCbb3), b_+215);
  CYC(b_+215, b_+217);
  s_seasonsFunc_03_69d1_hook(gb); return;

state4:
  CYC(b_+217, b_+220); A = mem_rd(gb, wFrameCounter);
  CYC(b_+220, b_+222); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+222); return; }
  CYC(b_+222, b_+223);
  CALL_C(b_+223, s_incCbbfAndCbb8_hook, SYM(incCbbfAndCbb8), b_+226);
  CYC(b_+226, b_+229); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+229, b_+230); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+230, b_+232); s_seasonsFunc_03_69d1_hook(gb); return; }
  CYC(b_+230, b_+232);
  CYC(b_+232, b_+234); A = 0xc7;
  CYC(b_+234, b_+237); mem_wr(gb, wGfxRegs1_WINY, A);
  CYC(b_+237, b_+240); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(b_+240, b_+242); A = 0x04;
  CYC(b_+242, b_+245); mem_wr(gb, wCutsceneState, A);
  CYC(b_+245, b_+247); B = 0x10;
  CYC(b_+247, b_+250); SET_HL(wTmpcbb3);
  CYC(b_+250, b_+253);
  TAIL(clearMemory);
}

// The ship's window at line wTmpcbb8.
void s_seasonsFunc_03_69d1_hook(GB *gb) {
  BASE(seasonsFunc_03_69d1);
  CYC(b_+0, b_+2); A = 0xa5;
  CYC(b_+2, b_+5); mem_wr(gb, wGfxRegs1_WINX, A);
  CYC(b_+5, b_+8); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+8, b_+11); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(b_+11, b_+14); mem_wr(gb, wGfxRegs1_WINY, A);
  CYC(b_+14, b_+17); mem_wr(gb, wTmpcbbc, A);
  RET(b_+17); return;
}

// Back inside the ship with the sick pirates; state 2 moves on to stage 5.
void s_cutsceneHandler_0c_stage4_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+28) { goto state1; }
    else if (jt_ == b_+29) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+10, b_+13); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CALL_L(b_+15, pirates_enter_ship, b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+22, b_+25); SET_HL(SYM(objectData_sickPiratiansInShip));
  CYC(b_+25, b_+28);
  TAIL(parseGivenObjectData_b00);

state1:
  RET(b_+28); return;

state2:
  CYC(b_+29, b_+31); A = 0x05;
  CALL_C(b_+31, s_seasonsFunc_03_67e9_hook, SYM(seasonsFunc_03_67e9), b_+34);
  CYC(b_+34, b_+37);
  TAIL(fadeoutToWhite);
}

// The west coast (room $0f2): the ship sails down into view with animated waves, docks, and
// Link warps to the coast.
void s_cutsceneHandler_0c_stage5_hook(GB *gb) {
  BASE(cutsceneHandler_0c_stage5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_03_6b6c_hook, SYM(seasonsFunc_03_6b6c), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (pirates_jump_table(gb));
    if (jt_ == b_+13) { goto state0; }
    else if (jt_ == b_+82) { goto state1; }
    else if (jt_ == b_+109) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+13, b_+16); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, s_disableLcd, SYM(disableLcd), b_+21);
  CALL_C(b_+21, s_clearScreenVariablesAndWramBank1, SYM(clearScreenVariablesAndWramBank1), b_+24);
  CALL_C(b_+24, s_incCbb3_hook, SYM(incCbb3), b_+27);
  CYC(b_+27, b_+29); A = 0x90;
  CYC(b_+29, b_+32); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+32, b_+35); mem_wr(gb, wTmpcbbf, A);
  CYC(b_+35, b_+37); A = 0x10;
  CYC(b_+37, b_+40); mem_wr(gb, wTmpcbbd, A);
  CYC(b_+40, b_+42); A = 0x03;
  CYC(b_+42, b_+45); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+45, b_+48); SET_BC(0x00f2); // ROOM_SEASONS_0f2
  CALL_L(b_+48, pirates_load_room, b_+51);
  CYC(b_+51, b_+53); A = 0xff;
  CYC(b_+53, b_+56); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+56, b_+58); E = 0x00;
  CALL_C(b_+58, s_loadObjectGfxHeaderToSlot4, SYM(loadObjectGfxHeaderToSlot4), b_+61);
  CYC(b_+61, b_+63); A = 0x53; // GFXH_PIRATE_SHIP_ARRIVING_LAYOUT
  CALL_C(b_+63, s_loadGfxHeader, SYM(loadGfxHeader), b_+66);
  CYC(b_+66, b_+68); A = 0x54; // GFXH_PIRATE_SHIP_MOVING_EXTRA_TILES
  CALL_C(b_+68, s_loadGfxHeader, SYM(loadGfxHeader), b_+71);
  CYC(b_+71, b_+74); SET_HL(SYM(objectData_pirateShipEnteringWestCoast));
  CALL_C(b_+74, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+77);
  CYC(b_+77, b_+79); A = 0x12;
  CYC(b_+79, b_+82);
  pirates_fade_in(gb); return;

state1:
  CYC(b_+82, b_+85); A = mem_rd(gb, wFrameCounter);
  CYC(b_+85, b_+87); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+87, b_+89); goto state1Func0; }
  CYC(b_+87, b_+89);
  CALL_C(b_+89, s_decCbbfAndCbb8_hook, SYM(decCbbfAndCbb8), b_+92);
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+95, b_+98); goto state1Func0; }
  CYC(b_+95, b_+98);
  CALL_C(b_+98, s_seasonsFunc_03_6b30_hook, SYM(seasonsFunc_03_6b30), b_+101);
  CALL_C(b_+101, s_incCbb3_hook, SYM(incCbb3), b_+104);
state1Func0:
  CALL_C(b_+104, s_seasonsFunc_03_69d1_hook, SYM(seasonsFunc_03_69d1), b_+107);
  CYC(b_+107, b_+109);
  s_seasonsFunc_03_6aca_hook(gb); return;

state2:
  CYC(b_+109, b_+112); A = mem_rd(gb, wFrameCounter);
  CYC(b_+112, b_+114); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+114, b_+116); goto state1Func0; }
  CYC(b_+114, b_+116);
  CALL_C(b_+116, s_decCbbfAndCbb8_hook, SYM(decCbbfAndCbb8), b_+119);
  CYC(b_+119, b_+120); A = mem_rd(gb, HL);
  CYC(b_+120, b_+122); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+122, b_+124); goto state1Func0; }
  CYC(b_+122, b_+124);
  CALL_C(b_+124, s_seasonsFunc_03_6b30_hook, SYM(seasonsFunc_03_6b30), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+129); goto state1Func0; }
  CYC(b_+127, b_+129);
  CYC(b_+129, b_+131); A = 0x17; // GLOBALFLAG_PIRATE_SHIP_DOCKED
  CALL_C(b_+131, s_setGlobalFlag, SYM(setGlobalFlag), b_+134);
  CYC(b_+134, b_+135); alu_xor(gb, A);
  CYC(b_+135, b_+138); mem_wr(gb, wActiveMusic, A);
  CYC(b_+138, b_+141); SET_HL(b_+144); // @state2WarpDestVariables
  CYC(b_+141, b_+144);
  TAIL(setWarpDestVariables);
}

// Wave frame (wTmpcbbe - 1, wTmpcbb8 & 7) into VRAM $8ce0.
void s_seasonsFunc_03_6a9d_hook(GB *gb) {
  BASE(seasonsFunc_03_6a9d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wTmpcbbe);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  CYC(b_+8, b_+10); alu_and(gb, 0x03);
  CYC(b_+10, b_+13); SET_HL(SYM(seasonsTable_03_6b1a));
  CYC(b_+13, b_+14); pirates_add_double_index(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+20, b_+22); alu_and(gb, 0x07);
  CYC(b_+22, b_+23); pirates_add_double_index(gb, b_+23);
  CYC(b_+23, b_+26); SET_DE(w2Filler6 + 0x50);
  CALL_C(b_+26, s_seasonsFunc_03_6b22_hook, SYM(seasonsFunc_03_6b22), b_+29);
  CYC(b_+29, b_+31); A = 0x00;
  CYC(b_+31, b_+33); mem_wr(gb, IO_SVBK, A);
  CYC(b_+33, b_+36); SET_HL(w2Filler6 + 0x50);
  CYC(b_+36, b_+39); SET_DE(0x8ce0);
  CYC(b_+39, b_+42); SET_BC(0x0102);
  CYC(b_+42, b_+45);
  TAIL(queueDmaTransfer);
}

// Every 16 calls the next wave frame (wTmpcbbe) also goes to VRAM $88e1; otherwise only the
// $8ce0 frame is refreshed.
void s_seasonsFunc_03_6aca_hook(GB *gb) {
  BASE(seasonsFunc_03_6aca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbbd);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+6); s_seasonsFunc_03_6a9d_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x10);
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+12); mem_wr(gb, IO_SVBK, A);
  CYC(b_+12, b_+15); A = mem_rd(gb, wTmpcbbe);
  CYC(b_+15, b_+18); SET_HL(SYM(seasonsTable_03_6b1a));
  CYC(b_+18, b_+19); pirates_add_double_index(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); H = mem_rd(gb, HL);
  CYC(b_+21, b_+22); L = A;
  CYC(b_+22, b_+25); SET_DE(w2Filler6 + 0x30);
  CYC(b_+25, b_+26); push_effect(gb, HL);
  CALL_C(b_+26, s_seasonsFunc_03_6b22_hook, SYM(seasonsFunc_03_6b22), b_+29);
  CYC(b_+29, b_+30); SET_HL(pop_effect(gb));
  CYC(b_+30, b_+33); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+33, b_+35); alu_and(gb, 0x07);
  CYC(b_+35, b_+36); pirates_add_double_index(gb, b_+36);
  CYC(b_+36, b_+39); SET_DE(w2Filler6 + 0x50);
  CALL_C(b_+39, s_seasonsFunc_03_6b22_hook, SYM(seasonsFunc_03_6b22), b_+42);
  CYC(b_+42, b_+44); A = 0x00;
  CYC(b_+44, b_+46); mem_wr(gb, IO_SVBK, A);
  CYC(b_+46, b_+49); SET_HL(w2Filler6 + 0x30);
  CYC(b_+49, b_+52); SET_DE(0x88e1);
  CYC(b_+52, b_+55); SET_BC(0x0102);
  CALL_C(b_+55, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+58);
  CYC(b_+58, b_+61); SET_HL(w2Filler6 + 0x50);
  CYC(b_+61, b_+64); SET_DE(0x8ce0);
  CYC(b_+64, b_+67); SET_BC(0x0102);
  CALL_C(b_+67, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+70);
  CYC(b_+70, b_+73); A = mem_rd(gb, wTmpcbbe);
  CYC(b_+73, b_+74); A = alu_inc8(gb, A);
  CYC(b_+74, b_+76); alu_and(gb, 0x03);
  CYC(b_+76, b_+79); mem_wr(gb, wTmpcbbe, A);
  RET(b_+79); return;
}

// Two 16-byte rows from hl (the second $10 bytes after the first) to de.
void s_seasonsFunc_03_6b22_hook(GB *gb) {
  BASE(seasonsFunc_03_6b22);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x10;
  CALL_C(b_+2, s_copyMemory, SYM(copyMemory), b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0010);
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+11); B = 0x10;
  CYC(b_+11, b_+14);
  TAIL(copyMemory);
}

// The next frame of the ship arriving (seasonsTable_03_6b59, $ff ends it with Z).
void s_seasonsFunc_03_6b30_hook(GB *gb) {
  BASE(seasonsFunc_03_6b30);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+3, b_+6); SET_HL(SYM(seasonsTable_03_6b59));
  CYC(b_+6, b_+7); pirates_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+10); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  s_label_03_196_hook(gb); return;
}

// Frame hl (low byte in a): tiles and attributes queued to the window map, the window line
// reset to $10.
void s_label_03_196_hook(GB *gb) {
  BASE(label_03_196);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); L = A;
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+5); SET_DE(0x9c00);
  CYC(b_+5, b_+8); SET_BC(0x2102);
  CALL_C(b_+8, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+14); H = H | 0x04;
  CYC(b_+14, b_+16); E = 0x01;
  CALL_C(b_+16, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+19);
  CYC(b_+19, b_+21); A = 0x10;
  CYC(b_+21, b_+24); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+24, b_+27); SET_HL(wTmpcbb7);
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+28); return;
}

// The ship's mast sprite.
void s_seasonsFunc_03_6b6c_hook(GB *gb) {
  BASE(seasonsFunc_03_6b6c);
  CYC(b_+0, b_+3); SET_HL(SYM(oamData_03_6b72));
  CYC(b_+3, b_+6);
  TAIL(addSpritesToOam);
}

void s_incCbbfAndCbb8_hook(GB *gb) {
  BASE(incCbbfAndCbb8);
  CYC(b_+0, b_+3); SET_HL(wTmpcbbf);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); SET_HL(wTmpcbb8);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+8); return;
}

void s_decCbbfAndCbb8_hook(GB *gb) {
  BASE(decCbbfAndCbb8);
  CYC(b_+0, b_+3); SET_HL(wTmpcbbf);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); SET_HL(wTmpcbb8);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+8); return;
}
