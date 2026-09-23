#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/transitionToDragonOnox.s, bank $0f: the change from
// General Onox to Dragon Onox. wTmpcfc0+8 is the state, +9/+$0a the body tile frame to draw
// (bit 7 of +9 = redraw requested), +$0b the claw frame (bit 7 = redraw), +$0c/+$0d the scroll
// offsets, +$0e the window line. The dragon is drawn from bank 6 tile maps through
// wTmpVramBuffer into the bank 3 VRAM buffers (w3VramTiles, w3TileMappingIndices).

static uint16_t onox_jump_table(GB *gb) {
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

static void onox_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// seasonsFunc_0f_704d from its +9: redraw the claws, the gfx register state and the window line.
static void onox_redraw_claws_and_regs(GB *gb) {
  BASE(seasonsFunc_0f_704d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+9, b_+12); SET_HL(wTmpcfc0 + 0x0b);
  CYC(b_+12, b_+14); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+14, s_seasons_func_0f_722f_hook, SYM(seasons_func_0f_722f), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); mem_wr(gb, IO_SVBK, A);
  CYC(b_+20, b_+22); A = 0x02;
  CALL_C(b_+22, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcfc0 + 0x0e);
  CYC(b_+28, b_+31); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+31, b_+33); A = 0x06;
  CYC(b_+33, b_+35); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  RET(b_+35); return;
}

// seasonsFunc_0f_704d@state4: once the text waits for A and A is pressed, stop the text, clear
// the BG map (attributes to palette 5), raise the window's priority bits and redraw.
static void onox_state4(GB *gb) {
  BASE(seasonsFunc_0f_704d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+36, b_+39); A = mem_rd(gb, wTextIsActive);
  CYC(b_+39, b_+41); alu_cp(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+45, b_+47); alu_and(gb, 0x01); // BTN_A
  if (F & FZ) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CALL_C(b_+48, s_stopTextThread, SYM(stopTextThread), b_+51);
  CYC(b_+51, b_+53); A = 0x05;
  CYC(b_+53, b_+56); mem_wr(gb, wTmpcfc0 + 0x08, A);
  CALL_C(b_+56, s_disableLcd, SYM(disableLcd), b_+59);
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+62); mem_wr(gb, IO_VBK, A);
  CYC(b_+62, b_+65); SET_HL(0x9800);
  CYC(b_+65, b_+68); SET_BC(0x0400);
  CALL_C(b_+68, s_clearMemoryBc, SYM(clearMemoryBc), b_+71);
  CYC(b_+71, b_+73); A = 0x01;
  CYC(b_+73, b_+75); mem_wr(gb, IO_VBK, A);
  CYC(b_+75, b_+78); SET_HL(0x9800);
  CYC(b_+78, b_+81); SET_BC(0x0400);
  CYC(b_+81, b_+83); A = 0x0d;
  CALL_C(b_+83, s_fillMemoryBc, SYM(fillMemoryBc), b_+86);
  CYC(b_+86, b_+89); SET_HL(0x9f60);
  CYC(b_+89, b_+91); B = 0x40;
  for (;;) {
    CYC(b_+91, b_+92); A = mem_rd(gb, HL);
    CYC(b_+92, b_+94); alu_or(gb, 0x80);
    CYC(b_+94, b_+95); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+95, b_+96); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+96, b_+98); continue; }
    CYC(b_+96, b_+98);
    break;
  }
  CALL_C(b_+98, s_seasons_func_0f_71fb_hook, SYM(seasons_func_0f_71fb), b_+101);
  CYCT(b_+101, b_+103);
  onox_redraw_claws_and_regs(gb); return;
}

// seasonsFunc_0f_70b4_swapGraphics@state3: redraw the body frame (+9 bit 7) or the claws (+$0b
// bit 7) when requested.
static void onox_state3(GB *gb) {
  BASE(seasonsFunc_0f_70b4_swapGraphics);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+84, b_+87); SET_HL(wTmpcfc0 + 0x09);
  CYC(b_+87, b_+89); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+89, b_+91); goto body; }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); L = 0xcb; // wTmpcfc0+$0b
  CYC(b_+93, b_+95); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+98); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+98, s_seasons_func_0f_722f_hook, SYM(seasons_func_0f_722f), b_+101);
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+104); mem_wr(gb, IO_SVBK, A);
  RET(b_+104); return;
body:
  CYC(b_+105, b_+106); B = mem_rd(gb, HL);
  CYC(b_+106, b_+107); alu_xor(gb, A);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+108, b_+110); B = B & 0x7f;
  CYC(b_+110, b_+111); mem_wr(gb, HL, B);
  CALL_C(b_+111, s_seasons_func_0f_712a_hook, SYM(seasons_func_0f_712a), b_+114);
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CYC(b_+115, b_+117); mem_wr(gb, IO_SVBK, A);
  RET(b_+117); return;
}

// seasons_func_0f_712a@state5: once scrolled far enough, the fight is won: Onox's room flag, the
// season locked to spring, and a fade to white.
static void onox_state5(GB *gb) {
  BASE(seasons_func_0f_712a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+9, b_+12); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+12, b_+14); alu_cp(gb, 0x78);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x06;
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0 + 0x08, A);
  CYC(b_+20, b_+23); SET_HL(wGroup5RoomFlags + 0x91);
  CYC(b_+23, b_+25); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+25, b_+27); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+27, setGlobalFlag_hook, SYM(setGlobalFlag), b_+30);
  CYC(b_+30, b_+33);
  TAIL_SG(fadeoutToWhite);
}

// seasons_func_0f_712a@state6: after the fade, the Din crystal cutscene.
static void onox_state6(GB *gb) {
  BASE(seasons_func_0f_712a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+33, b_+36); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CALL_C(b_+38, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+41);
  CYC(b_+41, b_+43); A = 0x09; // CUTSCENE_S_DIN_CRYSTAL_DESCENDING
  CYC(b_+43, b_+46); mem_wr(gb, wCutsceneTrigger, A);
  RET(b_+46); return;
}

// The transition's state machine: 0 set the scene up, 1 wait, 2 spawn Dragon Onox, 3 redraw on
// request, 4 wait for the text, 5 wait for the scroll, 6 hand over to the next cutscene.
void s_seasonsFunc_0f_6f75_hook(GB *gb) {
  BASE(seasonsFunc_0f_6f75);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (onox_jump_table(gb));
    if (jt_ == b_+18) { goto state0; }
    else if (jt_ == b_+184) { goto ret; }
    else if (jt_ == b_+185) { goto state2; }
    else if (jt_ == SYM(seasonsFunc_0f_70b4_swapGraphics) + 84) { onox_state3(gb); return; }
    else if (jt_ == SYM(seasonsFunc_0f_704d) + 36) { onox_state4(gb); return; }
    else if (jt_ == SYM(seasons_func_0f_712a) + 9) { onox_state5(gb); return; }
    else if (jt_ == SYM(seasons_func_0f_712a) + 33) { onox_state6(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+18, b_+21); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, s_clearWramBank1, SYM(clearWramBank1), b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CALL_C(b_+27, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+30);
  CALL_C(b_+30, s_getFreeEnemySlot_uncounted, SYM(getFreeEnemySlot_uncounted), b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x02); // ENEMY_GENERAL_ONOX
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x02);
  CALL_C(b_+38, getFreePartSlot_hook, SYM(getFreePartSlot), b_+41);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x48); // PART_48
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x04);
  CYC(b_+46, b_+49); SET_HL(w1Link);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x03);
  CYC(b_+51, b_+53); L = 0x0b; // w1Link.yh
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x28);
  CYC(b_+55, b_+57); L = 0x0d; // w1Link.xh
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x50);
  CYC(b_+59, b_+61); A = 0x30;
  CYC(b_+61, b_+63); mem_wr(gb, hCameraY, A);
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+66); mem_wr(gb, hCameraX, A);
  CYC(b_+66, b_+69); SET_HL(wRoomCollisions);
  CYC(b_+69, b_+72); SET_BC(0x00c0);
  CALL_C(b_+72, s_clearMemoryBc, SYM(clearMemoryBc), b_+75);
  CYC(b_+75, b_+78); SET_HL(wRoomCollisions + 0xa0);
  CYC(b_+78, b_+80); B = 0x10;
  CYC(b_+80, b_+82); A = 0x0f;
  CALL_C(b_+82, s_fillMemory, SYM(fillMemory), b_+85);
  CYC(b_+85, b_+88); SET_HL(wRoomCollisions + 0x0a);
  CYC(b_+88, b_+91); SET_BC(0x0b02);
  for (;;) {
    CYC(b_+91, b_+93); mem_wr(gb, HL, 0x0f);
    CYC(b_+93, b_+94); A = L;
    CYC(b_+94, b_+96); alu_add(gb, 0x10);
    CYC(b_+96, b_+97); L = A;
    CYC(b_+97, b_+98); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+98, b_+100); continue; }
    CYC(b_+98, b_+100);
    CYC(b_+100, b_+103); SET_HL(wRoomCollisions + 0x0f);
    CYC(b_+103, b_+105); B = 0x0b;
    CYC(b_+105, b_+106); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+106, b_+108); continue; }
    CYC(b_+106, b_+108);
    break;
  }
  CYC(b_+108, b_+111); SET_HL(wRoomLayout);
  CYC(b_+111, b_+114); SET_BC(0x00c0);
  CALL_C(b_+114, s_clearMemoryBc, SYM(clearMemoryBc), b_+117);
  CYC(b_+117, b_+118); alu_xor(gb, A);
  CYC(b_+118, b_+121); mem_wr(gb, wTmpcfc0 + 0x0a, A);
  CYC(b_+121, b_+124); mem_wr(gb, wTmpcfc0 + 0x0b, A);
  CYC(b_+124, b_+126); A = 0x80;
  CYC(b_+126, b_+129); mem_wr(gb, wTmpcfc0 + 0x0e, A);
  CYC(b_+129, b_+132); mem_wr(gb, wLinkInAir, A);
  CYC(b_+132, b_+135); SET_HL(wTmpcfc0 + 0x08);
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+136, s_disableLcd, SYM(disableLcd), b_+139);
  CALL_C(b_+139, s_seasonsFunc_0f_70b4_swapGraphics_hook, SYM(seasonsFunc_0f_70b4_swapGraphics), b_+142);
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+145); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+145, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+148);
  CALL_C(b_+148, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+151);
  CYC(b_+151, b_+153); A = 0x02;
  CALL_C(b_+153, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+156);
  CYC(b_+156, b_+159); A = mem_rd(gb, wTmpcfc0 + 0x0e);
  CYC(b_+159, b_+162); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+162, b_+164); A = 0x06;
  CYC(b_+164, b_+166); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+166, b_+169); SET_HL(wGfxRegs6_LCDC);
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x8f);
  CYC(b_+171, b_+172); L = alu_inc8(gb, L);
  CYC(b_+172, b_+174); mem_wr(gb, HL, 0x58); // SCY
  CYC(b_+174, b_+175); L = alu_inc8(gb, L);
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x00); // SCX
  CYC(b_+177, b_+178); L = alu_inc8(gb, L);
  CYC(b_+178, b_+180); A = 0xc7;
  CYC(b_+180, b_+181); mem_wr(gb, HL, A); SET_HL(HL + 1); // WINY
  CYC(b_+181, b_+182); mem_wr(gb, HL, A); SET_HL(HL + 1); // WINX
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x80); // LYC
ret:
  RET(b_+184); return;

state2:
  CALL_C(b_+185, s_clearEnemies, SYM(clearEnemies), b_+188);
  CALL_C(b_+188, s_getFreeEnemySlot, SYM(getFreeEnemySlot), b_+191);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x05); // ENEMY_DRAGON_ONOX
  CYC(b_+193, b_+196); SET_HL(wTmpcfc0 + 0x0a);
  CALL_C(b_+196, s_seasons_func_0f_712a_hook, SYM(seasons_func_0f_712a), b_+199);
  CYC(b_+199, b_+202); SET_HL(wTmpcfc0 + 0x0b);
  CYC(b_+202, b_+204); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+204, s_seasons_func_0f_722f_hook, SYM(seasons_func_0f_722f), b_+207);
  CYC(b_+207, b_+208); alu_xor(gb, A);
  CYC(b_+208, b_+210); mem_wr(gb, IO_SVBK, A);
  CYC(b_+210, b_+212); A = 0x03;
  CYC(b_+212, b_+215); mem_wr(gb, wTmpcfc0 + 0x08, A);
  RET(b_+215); return;
}

// Redraws the whole dragon: graphics swapped, body frame, claws and registers.
void s_seasonsFunc_0f_704d_hook(GB *gb) {
  BASE(seasonsFunc_0f_704d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_0f_70b4_swapGraphics_hook, SYM(seasonsFunc_0f_70b4_swapGraphics), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcfc0 + 0x0a);
  CALL_C(b_+6, s_seasons_func_0f_712a_hook, SYM(seasons_func_0f_712a), b_+9);
  onox_redraw_claws_and_regs(gb); return;
}

// The dragon palette, a blank BG map and window (attributes palette 5), empty bank 3 VRAM
// buffers, and the dragon's graphics.
void s_seasonsFunc_0f_70b4_swapGraphics_hook(GB *gb) {
  BASE(seasonsFunc_0f_70b4_swapGraphics);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x8d; // PALH_BG_DRAGON_ONOX
  CALL_C(b_+2, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+8); mem_wr(gb, IO_VBK, A);
  CYC(b_+8, b_+11); SET_HL(0x9800);
  CYC(b_+11, b_+14); SET_BC(0x0400);
  CALL_C(b_+14, s_clearMemoryBc, SYM(clearMemoryBc), b_+17);
  CYC(b_+17, b_+20); SET_HL(0x9f20);
  CYC(b_+20, b_+22); B = 0xa0;
  CALL_C(b_+22, s_clearMemory, SYM(clearMemory), b_+25);
  CYC(b_+25, b_+27); A = 0x01;
  CYC(b_+27, b_+29); mem_wr(gb, IO_VBK, A);
  CYC(b_+29, b_+32); SET_HL(0x9800);
  CYC(b_+32, b_+35); SET_BC(0x0400);
  CYC(b_+35, b_+37); A = 0x0d;
  CALL_C(b_+37, s_fillMemoryBc, SYM(fillMemoryBc), b_+40);
  CYC(b_+40, b_+43); SET_HL(0x9f20);
  CYC(b_+43, b_+45); B = 0xa0;
  CYC(b_+45, b_+47); A = 0x0d;
  CALL_C(b_+47, s_fillMemory, SYM(fillMemory), b_+50);
  CYC(b_+50, b_+52); A = 0x03;
  CYC(b_+52, b_+54); mem_wr(gb, IO_SVBK, A);
  CYC(b_+54, b_+57); SET_HL(w3VramTiles);
  CYC(b_+57, b_+60); SET_BC(0x02c0);
  CALL_C(b_+60, s_clearMemoryBc, SYM(clearMemoryBc), b_+63);
  CYC(b_+63, b_+66); SET_HL(w3TileMappingIndices);
  CYC(b_+66, b_+69); SET_BC(0x02c0);
  CYC(b_+69, b_+71); A = 0x0d;
  CALL_C(b_+71, s_fillMemoryBc, SYM(fillMemoryBc), b_+74);
  CYC(b_+74, b_+76); A = 0x96; // GFXH_DRAGON_ONOX
  CALL_C(b_+76, s_loadGfxHeader, SYM(loadGfxHeader), b_+79);
  CYC(b_+79, b_+81); A = 0x30; // UNCMP_GFXH_30
  CYC(b_+81, b_+84);
  TAIL_SG(loadUncompressedGfxHeader);
}

// Draws body frame (hl): frames 0..5 one way, 6.. the other.
void s_seasons_func_0f_712a_hook(GB *gb) {
  BASE(seasons_func_0f_712a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+3); alu_cp(gb, 0x06);
  if (F & FC) { CYCT(b_+3, b_+6); TAIL_S(seasonsFunc_0f_71cf_copyw6Filler1IntoWramBank3); }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9);
  TAIL_S(seasons_func_0f_71fb);
}

// Counts the screen shake counters down (unless +$0f is set), then eases the camera so Link
// sits $40 below its top, within 0..$30.
void s_seasonsFunc_0f_7159_hook(GB *gb) {
  BASE(seasonsFunc_0f_7159);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0f);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wScreenShakeCounterY);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto x; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
x:
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+16, b_+18); goto camera; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
camera:
  CYC(b_+19, b_+22); SET_HL(hCameraY);
  CYC(b_+22, b_+25); A = mem_rd(gb, w1Link_yh);
  CYC(b_+25, b_+26); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+26, b_+28); alu_cp(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  if (F & FC) { CYCT(b_+30, b_+32); goto up; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_cp(gb, 0x30);
  if (!(F & FC)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+36); return;
up:
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+40); return;
}

// Scroll registers from the camera and +$0c/+$0d, the window line, and a random jitter while
// the screen shakes.
void s_seasonsFunc_0f_7182_hook(GB *gb) {
  BASE(seasonsFunc_0f_7182);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x0c);
  CYC(b_+3, b_+5); A = mem_rd(gb, hCameraY);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+10); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+16); mem_wr(gb, wGfxRegs2_SCY + 1, A); // SCX
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+19); alu_add(gb, 0x28);
  CYC(b_+19, b_+22); mem_wr(gb, wGfxRegs6_SCY, A);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+26); mem_wr(gb, wGfxRegs6_SCY + 1, A); // SCX
  CYC(b_+26, b_+27); alu_sub(gb, B);
  CYC(b_+27, b_+29); alu_sub(gb, 0x50);
  CYC(b_+29, b_+31); alu_cp(gb, 0x90);
  if (F & FC) { CYCT(b_+31, b_+33); goto lyc; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); A = 0xc7;
lyc:
  CYC(b_+35, b_+38); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+38, b_+41); mem_wr(gb, wTmpcfc0 + 0x0e, A);
  CYC(b_+41, b_+44); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CALL_C(b_+46, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x03);
  CYC(b_+51, b_+54); SET_HL(SYM(table_71cb));
  CYC(b_+54, b_+55); onox_add_a_to_hl(gb, b_+55);
  CYC(b_+55, b_+58); A = mem_rd(gb, wGfxRegs2_LYC);
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+59, b_+61); alu_cp(gb, 0x90);
  if (!(F & FC)) { RET_TAKEN(b_+61); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+65); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+65, b_+68); A = mem_rd(gb, wGfxRegs6_SCY);
  CYC(b_+68, b_+69); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+69, b_+72); mem_wr(gb, wGfxRegs6_SCY, A);
  RET(b_+72); return;
}

// Body frames 0..5: the frame's two bank 6 maps (tiles, attributes) through wTmpVramBuffer into
// the bank 3 buffers, the attributes' mirrored halves flipped.
void s_seasonsFunc_0f_71cf_copyw6Filler1IntoWramBank3_hook(GB *gb) {
  BASE(seasonsFunc_0f_71cf_copyw6Filler1IntoWramBank3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_load5aIntoBc_hook, SYM(load5aIntoBc), b_+3);
  CYC(b_+3, b_+4); push_effect(gb, BC);
  CYC(b_+4, b_+7); SET_HL(w6DragonOnoxTileMap1);
  CYC(b_+7, b_+8); alu_add_hl(gb, BC);
  CYC(b_+8, b_+10); B = 0x50;
  CALL_C(b_+10, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+13);
  CYC(b_+13, b_+16); SET_HL(w3VramTiles + 0x02);
  CYC(b_+16, b_+18); C = 0x00;
  CALL_C(b_+18, s_seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3_hook, SYM(seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3), b_+21);
  CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
  CYC(b_+22, b_+25); SET_HL(w6DragonOnoxTileAttr1);
  CYC(b_+25, b_+26); alu_add_hl(gb, BC);
  CYC(b_+26, b_+28); B = 0x50;
  CALL_C(b_+28, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+31);
  CYC(b_+31, b_+34); SET_HL(w3TileMappingIndices + 0x02);
  CYC(b_+34, b_+36); C = 0x20;
  CALL_C(b_+36, s_seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3_hook, SYM(seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3), b_+39);
  CYC(b_+39, b_+41); A = 0x2e; // UNCMP_GFXH_2e
  CYC(b_+41, b_+44);
  TAIL_SG(loadUncompressedGfxHeader);
}

// Body frames 6..8 (later ones as 8): the bigger frames.
void s_seasons_func_0f_71fb_hook(GB *gb) {
  BASE(seasons_func_0f_71fb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+3, b_+5); alu_sub(gb, 0x06);
  CYC(b_+5, b_+7); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+7, b_+9); goto frame; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x02;
frame:
  CYC(b_+11, b_+12); alu_add(gb, A);
  CALL_C(b_+12, s_load5aIntoBc_hook, SYM(load5aIntoBc), b_+15);
  CYC(b_+15, b_+16); push_effect(gb, BC);
  CYC(b_+16, b_+19); SET_HL(w6DragonOnoxTileMap1 + 0x1e0);
  CYC(b_+19, b_+20); alu_add_hl(gb, BC);
  CYC(b_+20, b_+22); B = 0xa0;
  CALL_C(b_+22, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+25);
  CYC(b_+25, b_+28); SET_HL(w3VramTiles + 0x02);
  CALL_C(b_+28, s_seasons_func_0f_72d1_hook, SYM(seasons_func_0f_72d1), b_+31);
  CYC(b_+31, b_+32); SET_BC(pop_effect(gb));
  CYC(b_+32, b_+35); SET_HL(w6DragonOnoxTileAttr1 + 0x1e0);
  CYC(b_+35, b_+36); alu_add_hl(gb, BC);
  CYC(b_+36, b_+38); B = 0xa0;
  CALL_C(b_+38, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+41);
  CYC(b_+41, b_+44); SET_HL(w3TileMappingIndices + 0x02);
  CALL_C(b_+44, s_seasons_func_0f_72dc_hook, SYM(seasons_func_0f_72dc), b_+47);
  CYC(b_+47, b_+49); A = 0x2e; // UNCMP_GFXH_2e
  CYC(b_+49, b_+52);
  TAIL_SG(loadUncompressedGfxHeader);
}

// The claws: the bank 3 claw area cleared, then claw frame +$0b (frames 3.. mirrored) copied in
// at its row offset from table_7291.
void s_seasons_func_0f_722f_hook(GB *gb) {
  BASE(seasons_func_0f_722f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); SET_HL(w3VramTiles + 0x140);
  CYC(b_+7, b_+9); B = 0xe0;
  CALL_C(b_+9, s_clearMemory, SYM(clearMemory), b_+12);
  CYC(b_+12, b_+15); SET_HL(w3TileMappingIndices + 0x140);
  CYC(b_+15, b_+17); B = 0xe0;
  CYC(b_+17, b_+19); A = 0x0d;
  CALL_C(b_+19, s_fillMemory, SYM(fillMemory), b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+25, b_+27); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+27, b_+29); goto frame; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); alu_sub(gb, 0x03);
frame:
  CYC(b_+31, b_+32); alu_add(gb, A);
  CYC(b_+32, b_+33); alu_add(gb, A);
  CYC(b_+33, b_+35); A = alu_swap(gb, A);
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+38); alu_and(gb, 0xf0);
  CYC(b_+38, b_+39); C = A;
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); alu_and(gb, 0x0f);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+44); push_effect(gb, BC);
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+47, b_+50); SET_HL(SYM(table_7291));
  CYC(b_+50, b_+51); onox_add_a_to_hl(gb, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); mem_wr(gb, hFF8B, A);
  CYC(b_+54, b_+57); SET_HL(w6DragonOnoxTileMap2);
  CYC(b_+57, b_+58); alu_add_hl(gb, BC);
  CYC(b_+58, b_+60); B = 0x40;
  CALL_C(b_+60, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+63);
  CYC(b_+63, b_+65); A = mem_rd(gb, hFF8B);
  CYC(b_+65, b_+67); alu_add(gb, 0x40);
  CYC(b_+67, b_+68); L = A;
  CYC(b_+68, b_+70); H = (uint8_t)((w3VramTiles + 0x100) >> 8);
  CALL_C(b_+70, s_seasons_func_0f_731a_hook, SYM(seasons_func_0f_731a), b_+73);
  CYC(b_+73, b_+74); SET_BC(pop_effect(gb));
  CYC(b_+74, b_+77); SET_HL(w6DragonOnoxTileMap2 + 0x300);
  CYC(b_+77, b_+78); alu_add_hl(gb, BC);
  CYC(b_+78, b_+80); B = 0x40;
  CALL_C(b_+80, s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook, SYM(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer), b_+83);
  CYC(b_+83, b_+85); A = mem_rd(gb, hFF8B);
  CYC(b_+85, b_+87); alu_add(gb, 0x40);
  CYC(b_+87, b_+88); L = A;
  CYC(b_+88, b_+90); H = (uint8_t)((w3TileMappingIndices + 0x100) >> 8);
  CALL_C(b_+90, s_seasons_func_0f_7325_hook, SYM(seasons_func_0f_7325), b_+93);
  CYC(b_+93, b_+95); A = 0x2f; // UNCMP_GFXH_2f
  CYC(b_+95, b_+98);
  TAIL_SG(loadUncompressedGfxHeader);
}

// Copies b bytes from hl in WRAM bank 6 into wTmpVramBuffer.
void s_seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer_hook(GB *gb) {
  BASE(seasonsFunc_0f_7297_copyw6Filler1IntowTmpVramBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x06;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); SET_DE(wTmpVramBuffer);
  for (;;) {
    CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+8, b_+9); mem_wr(gb, DE, A);
    CYC(b_+9, b_+10); E = alu_inc8(gb, E);
    CYC(b_+10, b_+11); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+11, b_+13); continue; }
    CYC(b_+11, b_+13);
    break;
  }
  RET(b_+13); return;
}

// wTmpVramBuffer into bank 3 at hl, rows of 16: 8 bytes as is, then the same 8 bytes reversed
// in pairs with c xored in (the mirrored half), until the buffer reaches $90.
void s_seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3_hook(GB *gb) {
  BASE(seasonsFunc_0f_72a5_copyFromwTmpVramBufferIntoBank3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); SET_DE(wTmpVramBuffer);
  for (;;) {
    CYC(b_+7, b_+9); B = 0x04;
    for (;;) {
      CYC(b_+9, b_+10); A = mem_rd(gb, DE);
      CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+11, b_+12); E = alu_inc8(gb, E);
      CYC(b_+12, b_+13); A = mem_rd(gb, DE);
      CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+14, b_+15); E = alu_inc8(gb, E);
      CYC(b_+15, b_+16); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+16, b_+18); continue; }
      CYC(b_+16, b_+18);
      break;
    }
    CYC(b_+18, b_+20); B = 0x04;
    for (;;) {
      CYC(b_+20, b_+21); E = alu_dec8(gb, E);
      CYC(b_+21, b_+22); A = mem_rd(gb, DE);
      CYC(b_+22, b_+23); alu_xor(gb, C);
      CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+24, b_+25); E = alu_dec8(gb, E);
      CYC(b_+25, b_+26); A = mem_rd(gb, DE);
      CYC(b_+26, b_+27); alu_xor(gb, C);
      CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+28, b_+29); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+29, b_+31); continue; }
      CYC(b_+29, b_+31);
      break;
    }
    CYC(b_+31, b_+32); A = E;
    CYC(b_+32, b_+34); alu_add(gb, 0x08);
    CYC(b_+34, b_+35); E = A;
    CYC(b_+35, b_+37); A = 0x10;
    CYC(b_+37, b_+38); onox_add_a_to_hl(gb, b_+38);
    CYC(b_+38, b_+39); A = E;
    CYC(b_+39, b_+41); alu_cp(gb, 0x90);
    if (!(F & FZ)) { CYCT(b_+41, b_+43); continue; }
    CYC(b_+41, b_+43);
    break;
  }
  RET(b_+43); return;
}

// seasons_func_0f_72d1 from its +20 (shared with _72dc): frames 9.. are drawn mirrored, rows
// of 16 read backwards from $cd4f with c xored in.
static void onox_body_rows_mirrored(GB *gb) {
  BASE(seasons_func_0f_72d1);
  CYC(b_+20, b_+22); A = 0x03;
  CYC(b_+22, b_+24); mem_wr(gb, IO_SVBK, A);
  CYC(b_+24, b_+27); SET_DE(wTmpVramBuffer + 0x0f);
  for (;;) {
    CYC(b_+27, b_+29); B = 0x10;
    for (;;) {
      CYC(b_+29, b_+30); A = mem_rd(gb, DE);
      CYC(b_+30, b_+31); alu_xor(gb, C);
      CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+32, b_+33); E = alu_dec8(gb, E);
      CYC(b_+33, b_+34); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+34, b_+36); continue; }
      CYC(b_+34, b_+36);
      break;
    }
    CYC(b_+36, b_+37); A = E;
    CYC(b_+37, b_+39); alu_add(gb, 0x20);
    CYC(b_+39, b_+40); E = A;
    CYC(b_+40, b_+42); A = 0x10;
    CYC(b_+42, b_+43); onox_add_a_to_hl(gb, b_+43);
    CYC(b_+43, b_+44); A = E;
    CYC(b_+44, b_+46); alu_cp(gb, 0xef);
    if (!(F & FZ)) { CYCT(b_+46, b_+48); continue; }
    CYC(b_+46, b_+48);
    break;
  }
  RET(b_+48); return;
}

// seasons_func_0f_72d1 from its +49: frames 6..8, ten rows of 16 copied straight.
static void onox_body_rows(GB *gb) {
  BASE(seasons_func_0f_72d1);
  CYC(b_+49, b_+51); A = 0x03;
  CYC(b_+51, b_+53); mem_wr(gb, IO_SVBK, A);
  CYC(b_+53, b_+56); SET_DE(wTmpVramBuffer);
  CYC(b_+56, b_+58); C = 0x0a;
  for (;;) {
    CYC(b_+58, b_+60); B = 0x10;
    for (;;) {
      CYC(b_+60, b_+61); A = mem_rd(gb, DE);
      CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+62, b_+63); E = alu_inc8(gb, E);
      CYC(b_+63, b_+64); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+64, b_+66); continue; }
      CYC(b_+64, b_+66);
      break;
    }
    CYC(b_+66, b_+68); A = 0x10;
    CYC(b_+68, b_+69); onox_add_a_to_hl(gb, b_+69);
    CYC(b_+69, b_+70); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+70, b_+72); continue; }
    CYC(b_+70, b_+72);
    break;
  }
  RET(b_+72); return;
}

// The big body frame's tiles (xor 0).
void s_seasons_func_0f_72d1_hook(GB *gb) {
  BASE(seasons_func_0f_72d1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+3, b_+5); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+5, b_+7); onox_body_rows(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x00;
  CYCT(b_+9, b_+11);
  onox_body_rows_mirrored(gb); return;
}

// The big body frame's attributes (mirrored ones flipped with $20).
void s_seasons_func_0f_72dc_hook(GB *gb) {
  BASE(seasons_func_0f_72dc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+3, b_+5); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+5, b_+7); onox_body_rows(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x20;
  onox_body_rows_mirrored(gb); return;
}

// seasons_func_0f_731a from its +20 (shared with _7325): claw frames 3.., rows of 8 read
// backwards from $cd47 in pairs with c xored in.
static void onox_claw_rows_mirrored(GB *gb) {
  BASE(seasons_func_0f_731a);
  CYC(b_+20, b_+22); A = 0x03;
  CYC(b_+22, b_+24); mem_wr(gb, IO_SVBK, A);
  CYC(b_+24, b_+27); SET_DE(wTmpVramBuffer + 0x07);
  for (;;) {
    CYC(b_+27, b_+29); B = 0x04;
    for (;;) {
      CYC(b_+29, b_+30); A = mem_rd(gb, DE);
      CYC(b_+30, b_+31); alu_xor(gb, C);
      CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+32, b_+33); E = alu_dec8(gb, E);
      CYC(b_+33, b_+34); A = mem_rd(gb, DE);
      CYC(b_+34, b_+35); alu_xor(gb, C);
      CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+36, b_+37); E = alu_dec8(gb, E);
      CYC(b_+37, b_+38); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+38, b_+40); continue; }
      CYC(b_+38, b_+40);
      break;
    }
    CYC(b_+40, b_+41); A = E;
    CYC(b_+41, b_+43); alu_add(gb, 0x10);
    CYC(b_+43, b_+44); E = A;
    CYC(b_+44, b_+46); A = 0x18;
    CYC(b_+46, b_+47); onox_add_a_to_hl(gb, b_+47);
    CYC(b_+47, b_+48); A = E;
    CYC(b_+48, b_+50); alu_cp(gb, 0x7f);
    if (!(F & FZ)) { CYCT(b_+50, b_+52); continue; }
    CYC(b_+50, b_+52);
    break;
  }
  RET(b_+52); return;
}

// seasons_func_0f_731a from its +53: claw frames 0..2, rows of 8 copied straight.
static void onox_claw_rows(GB *gb) {
  BASE(seasons_func_0f_731a);
  CYC(b_+53, b_+55); A = 0x03;
  CYC(b_+55, b_+57); mem_wr(gb, IO_SVBK, A);
  CYC(b_+57, b_+60); SET_DE(wTmpVramBuffer);
  for (;;) {
    CYC(b_+60, b_+62); B = 0x04;
    for (;;) {
      CYC(b_+62, b_+63); A = mem_rd(gb, DE);
      CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+64, b_+65); E = alu_inc8(gb, E);
      CYC(b_+65, b_+66); A = mem_rd(gb, DE);
      CYC(b_+66, b_+67); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+67, b_+68); E = alu_inc8(gb, E);
      CYC(b_+68, b_+69); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+69, b_+71); continue; }
      CYC(b_+69, b_+71);
      break;
    }
    CYC(b_+71, b_+73); A = 0x18;
    CYC(b_+73, b_+74); onox_add_a_to_hl(gb, b_+74);
    CYC(b_+74, b_+75); A = E;
    CYC(b_+75, b_+77); alu_cp(gb, 0x78);
    if (!(F & FZ)) { CYCT(b_+77, b_+79); continue; }
    CYC(b_+77, b_+79);
    break;
  }
  RET(b_+79); return;
}

// The claw frame's tiles (xor 0).
void s_seasons_func_0f_731a_hook(GB *gb) {
  BASE(seasons_func_0f_731a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+5, b_+7); onox_claw_rows(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x00;
  CYCT(b_+9, b_+11);
  onox_claw_rows_mirrored(gb); return;
}

// The claw frame's attributes (mirrored ones flipped with $20).
void s_seasons_func_0f_7325_hook(GB *gb) {
  BASE(seasons_func_0f_7325);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+5, b_+7); onox_claw_rows(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x20;
  onox_claw_rows_mirrored(gb); return;
}

// bc = a * 5 * 16 (the offset of frame a).
void s_load5aIntoBc_hook(GB *gb) {
  BASE(load5aIntoBc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+3); alu_add(gb, A);
  CYC(b_+3, b_+4); alu_add(gb, B);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); alu_and(gb, 0xf0);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); B = A;
  RET(b_+14); return;
}
