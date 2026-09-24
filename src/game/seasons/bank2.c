#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank2.s, the Seasons-only pieces.

// NZ when the tileset has a season to show in the subscreen (wTilesetFlags bits 2..7).
void s_checkWhetherToDisplaySeasonInSubscreen_hook(GB *gb) {
  BASE(checkWhetherToDisplaySeasonInSubscreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0xfc);
  RET(b_+5); return;
}

static void bank2_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// Separate popups for each season
void s_minimapPopupType_templeOfSeasons_hook(GB *gb) {
  BASE(minimapPopupType_templeOfSeasons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x11;
  CYC(b_+2, b_+5); A = mem_rd(gb, wTmpcbb6);
  CYC(b_+5, b_+7); alu_cp(gb, 0x28);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_622f; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+12); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+12, b_+14); goto L_622f; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+17); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+17, b_+19); goto L_622f; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
L_622f:
  CYC(b_+20, b_+21); A = E;
  RET(b_+21); return;
}

// Suppress the pirate ship popup depending on whether it's moved.
void s_minimapPopupType_pirateShip_hook(GB *gb) {
  BASE(minimapPopupType_pirateShip);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkPirateShipMoved_hook, SYM(checkPirateShipMoved), b_+3);
  CYC(b_+3, b_+4); A = E;
  CYC(b_+4, b_+7); SET_HL(b_+25 /* @shipBefore */);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_623d; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(b_+29 /* @shipAfter */);
L_623d:
  CYC(b_+12, b_+15); A = mem_rd(gb, wTmpcbb6);
  CYC(b_+15, b_+16); C = A;
L_6241:
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); TAIL(minimapNoPopup); }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto L_6241; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = E;
  RET(b_+24); return;
}

// Seasons only: draw the locations of the jewels if Link has the treasure map.
void s_mapMenu_drawJewelLocations_hook(GB *gb) {
  BASE(mapMenu_drawJewelLocations);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(b_+89 /* @sprite */);
  CYC(b_+3, b_+6); SET_HL(wTmpcec0);
  CYC(b_+6, b_+8); B = 0x05;
  CALL_C(b_+8, s_copyMemoryReverse, SYM(copyMemoryReverse), b_+11);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_VAR03;
  CYC(b_+13, b_+16); A = mem_rd(gb, wFrameCounter);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+21); alu_and(gb, 0x03);
  CYC(b_+21, b_+22); alu_add(gb, A);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+27, b_+28); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); A = 0x4b;
  CALL_C(b_+31, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+34);
  if (!(F & FC)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38); SET_BC(0x0400);
drawTreasure:
  CYC(b_+38, b_+39); A = C;
  CYC(b_+39, b_+41); alu_add(gb, 0x4c);
  CALL_C(b_+41, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+44);
  if (F & FC) { CYCT(b_+44, b_+46); goto nextTreasure; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); A = C;
  CYC(b_+47, b_+50); SET_HL(wInsertedJewels);
  CALL_C(b_+50, s_checkFlag, SYM(checkFlag), b_+53);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto nextTreasure; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); push_effect(gb, BC);
  CALL_C(b_+56, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+59);
  CYC(b_+59, b_+60); A = C;
  if (F & FZ) { CYCT(b_+60, b_+62); goto L_6652; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); alu_add(gb, 0x04);
L_6652:
  CYC(b_+64, b_+67); SET_HL(b_+81 /* @jewelLocations */);
  CYC(b_+67, b_+68); bank2_d_add_a_to_hl(gb, b_+68);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+72); SET_HL(wTmpcec0);
  CALL_C(b_+72, s_mapMenu_drawSpriteAtRoomIndex, SYM(mapMenu_drawSpriteAtRoomIndex), b_+75);
  CYC(b_+75, b_+76); SET_BC(pop_effect(gb));
nextTreasure:
  CYC(b_+76, b_+77); C = alu_inc8(gb, C);
  CYC(b_+77, b_+78); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto drawTreasure; }
  CYC(b_+78, b_+80);
  RET(b_+80); return;
}

// @param[out]	zflag	nz if the pirate ship has moved
void s_checkPirateShipMoved_hook(GB *gb) {
  BASE(checkPirateShipMoved);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17;
  CYC(b_+2, b_+5);
  TAIL(checkGlobalFlag);
}

