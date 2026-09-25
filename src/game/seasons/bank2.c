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

static uint16_t bank2_d_jump_table(GB *gb) {
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

// inventoryMenuState2@func_02_57f3
static void bank2_d_func_02_57f3(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+162, b_+165); SET_HL(wTmpcbbf);
  CYC(b_+165, b_+166); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+166, b_+167); C = A;
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CYC(b_+168, b_+169); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+169, b_+171); goto countUp; }
  CYC(b_+169, b_+171);
  CYC(b_+171, b_+173); alu_add(gb, 0x02);
  CYC(b_+173, b_+174); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+174, b_+175); SET_HL(HL + 1);
  CYC(b_+175, b_+176); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+176, b_+178);
  goto drawBox;
countUp:
  CYC(b_+178, b_+179); SET_HL(HL + 1);
  CYC(b_+179, b_+180); A = mem_rd(gb, HL);
  CYC(b_+180, b_+182); alu_cp(gb, 0x04);
  if (!(F & FC)) { RET_TAKEN(b_+182); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+184); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
drawBox:
  CYC(b_+184, b_+186); L = 0xc0;
  CYC(b_+186, b_+187); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+187, b_+188); C = A;
  CYC(b_+188, b_+189); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+189, b_+190); B = A;
  CYC(b_+190, b_+191); A = mem_rd(gb, HL);
  CYC(b_+191, b_+194); SET_HL(w1ReservedInteraction0_var3f + 0x01);
  CYC(b_+194, b_+195); bank2_d_add_a_to_hl(gb, b_+195);
  CYC(b_+195, b_+198); SET_DE(0x0001);
  CALL_C(b_+198, s_fillRectangleInTilemap, SYM(fillRectangleInTilemap), b_+201);
  CYC(b_+201, b_+202); alu_scf(gb);
  RET(b_+202); return;
}

void s_inventoryMenuState2_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSubmenuState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bank2_d_jump_table(gb));
    if (jt_ == b_+10) goto subState0;
    if (jt_ == b_+73) goto subState1;
    if (jt_ == b_+95) goto subState2;
    HANDOFF(HL);
  } while (0);
subState0:
  CYC(b_+10, b_+13); SET_HL(wSatchelSelectedSeeds);
  CYC(b_+13, b_+16); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+16, b_+18); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+18, b_+20); goto readSeedType; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
readSeedType:
  CYC(b_+21, b_+22); E = mem_rd(gb, HL);
  CYC(b_+22, b_+24); D = 0x00;
findSeedIndex:
  CYC(b_+24, b_+25); A = D;
  CALL_C(b_+25, s_getSeedTypeInventoryIndex, SYM(getSeedTypeInventoryIndex), b_+28);
  CYC(b_+28, b_+29); alu_cp(gb, E);
  if (F & FZ) { CYCT(b_+29, b_+31); goto foundSeedIndex; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); D = alu_inc8(gb, D);
  CYC(b_+32, b_+34);
  goto findSeedIndex;
foundSeedIndex:
  CYC(b_+34, b_+35); A = D;
  CYC(b_+35, b_+38); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+38, b_+41); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+41, b_+44); SET_HL(b_+201);
  CYC(b_+44, b_+45); bank2_d_add_a_to_hl(gb, b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+49); SET_HL(wTmpcbbf);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+52, b_+53); A = alu_inc8(gb, A);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+54, b_+57); mem_wr(gb, wTmpcbbe, A);
  CYC(b_+57, b_+60); A = mem_rd(gb, wInventorySubmenu0CursorPos);
  CYC(b_+60, b_+62); alu_cp(gb, 0x08);
  CYC(b_+62, b_+64); A = 0x0a;
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto setTextPosition; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); alu_add(gb, 0xa0);
setTextPosition:
  CYC(b_+68, b_+69); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+69, b_+72); SET_HL(wSubmenuState);
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subState1:
  CYC(b_+73, b_+76); SET_HL(wTmpcbbe);
  CYC(b_+76, b_+77); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; }
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x02);
  CALL_L(b_+80, bank2_d_func_02_57f3, b_+83);
  if (F & FC) { CYCT(b_+83, b_+85); goto done; }
  CYC(b_+83, b_+85);
  CALL_C(b_+85, s_func_02_5a35, SYM(func_02_5a35), b_+88);
  CYC(b_+88, b_+91); SET_HL(wSubmenuState);
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
done:
  CYC(b_+92, b_+95);
  TAIL(func_02_55a8);
subState2:
  CYC(b_+95, b_+98); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+98, b_+100); alu_and(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+100, b_+102); goto buttonPressed; }
  CYC(b_+100, b_+102);
  CALL_C(b_+102, s_func_02_5938, SYM(func_02_5938), b_+105);
  CYC(b_+105, b_+108); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+108, b_+111); A = mem_rd(gb, wTmpcbb5);
  CALL_C(b_+111, s_getSeedTypeInventoryIndex, SYM(getSeedTypeInventoryIndex), b_+114);
  CYC(b_+114, b_+116); alu_add(gb, 0x20);
  CALL_C(b_+116, s_loadTreasureDisplayData, SYM(loadTreasureDisplayData_b00), b_+119);
  CYC(b_+119, b_+121); A = 0x06;
  CYC(b_+121, b_+122); bank2_d_add_a_to_hl(gb, b_+122);
  CYC(b_+122, b_+125); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+125, b_+127); alu_cp(gb, 0x13);
  CYC(b_+127, b_+129); A = 0x00;
  if (!(F & FZ)) { CYCT(b_+129, b_+131); goto showText; }
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+133); A = 0x05;
showText:
  CYC(b_+133, b_+134); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+134, s_showItemText2, SYM(showItemText2), b_+137);
  CYC(b_+137, b_+140);
  TAIL(func_02_5a35);
buttonPressed:
  CYC(b_+140, b_+143); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+143, b_+145); E = 0xbe;
  CYC(b_+145, b_+147); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+147, b_+149); goto equipSeed; }
  CYC(b_+147, b_+149);
  CYC(b_+149, b_+150); E = alu_inc8(gb, E);
equipSeed:
  CYC(b_+150, b_+153); A = mem_rd(gb, wTmpcbb5);
  CALL_C(b_+153, s_getSeedTypeInventoryIndex, SYM(getSeedTypeInventoryIndex), b_+156);
  CYC(b_+156, b_+158); D = 0xc6;
  CYC(b_+158, b_+159); mem_wr(gb, DE, A);
  CYC(b_+159, b_+162);
  TAIL(inventoryMenuState1__finalizeEquip);
}

void s_loadCommonGraphics_body_hook(GB *gb) {
  BASE(loadCommonGraphics_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_disableLcd, SYM(disableLcd), b_+3);
  CYC(b_+3, b_+5); A = 0x20;
  CALL_C(b_+5, s_loadGfxHeader, SYM(loadGfxHeader), b_+8);
  CYC(b_+8, b_+10); A = 0x83;
  CALL_C(b_+10, s_loadGfxHeader, SYM(loadGfxHeader), b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); C = 0xa5;
  CYC(b_+20, b_+22); alu_bit(gb, 3, B);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto loadMoneyGraphic; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_bit(gb, 7, B);
  if (F & FZ) { CYCT(b_+26, b_+28); goto updateDisplayedMoney; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); A = 0x10;
  CYC(b_+30, b_+32); C = 0xa7;
loadMoneyGraphic:
  CYC(b_+32, b_+33); push_effect(gb, BC);
  CYC(b_+33, b_+36); SET_HL(SYM(gfx_key_orechunk));
  CYC(b_+36, b_+37); bank2_d_add_a_to_hl(gb, b_+37);
  CYC(b_+37, b_+40); SET_DE(0x9090);
  CYC(b_+40, b_+43); SET_BC(0x001b);
  CALL_C(b_+43, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+46);
  CYC(b_+46, b_+47); SET_BC(pop_effect(gb));
updateDisplayedMoney:
  CYC(b_+47, b_+48); A = C;
  CYC(b_+48, b_+51); SET_HL(wDisplayedMoneyAddress);
  CYC(b_+51, b_+52); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+52, b_+53); mem_wr(gb, HL, C);
  if (F & FZ) { CYCT(b_+53, b_+55); goto refreshStatusBar; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); L = C;
  CYC(b_+56, b_+58); H = 0xc6;
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+62); mem_wr(gb, wDisplayedRupees, A);
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+66); mem_wr(gb, wDisplayedRupees + 0x01, A);
refreshStatusBar:
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CYC(b_+67, b_+70); mem_wr(gb, wcbe8, A);
  CALL_C(b_+70, s_updateStatusBar_body, SYM(updateStatusBar_body), b_+73);
  CYC(b_+73, b_+76);
  TAIL(checkReloadStatusBarGraphics);
}

void s_loadMinimapDisplayRoom_hook(GB *gb) {
  BASE(loadMinimapDisplayRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMinimapGroup);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto overworld; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+11, b_+13); goto makuTree; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+15, b_+17); goto group4; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19);
  goto setRoom;
overworld:
  CYC(b_+19, b_+20); A = C;
  CYC(b_+20, b_+22); alu_cp(gb, 0xc9);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto setRoom; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); C = 0x40;
  CYC(b_+26, b_+28);
  goto setRoom;
makuTree:
  CYC(b_+28, b_+31); SET_BC(0x00c9);
  CYC(b_+31, b_+33);
  goto setRoom;
group4:
  CYC(b_+33, b_+36); A = mem_rd(gb, wActiveGroup);
  CYC(b_+36, b_+38); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto setRoom; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+43); A = mem_rd(gb, wActiveRoom);
  CYC(b_+43, b_+45); alu_cp(gb, 0x98);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto setRoom; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); SET_BC(0x0000);
setRoom:
  CYC(b_+50, b_+51); A = C;
  CYC(b_+51, b_+54); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+54, b_+55); A = B;
  CYC(b_+55, b_+58); SET_HL(b_+64 /* @groupToDisplayMode */);
  CYC(b_+58, b_+59); bank2_d_add_a_to_hl(gb, b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+63); mem_wr(gb, wTmpcbb3, A);
  RET(b_+63); return;
}

void s_mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb) {
  BASE(mapGetRoomIndexWithoutUnusedColumns);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+7); A = mem_rd(gb, wTmpcbb6);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); push_effect(gb, BC);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); alu_and(gb, 0xf0);
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+15); C = A;
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+21); alu_sub(gb, C);
  CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
  CYC(b_+22, b_+23); alu_scf(gb);
  RET(b_+23); return;
}

void s_minimapPopupType_shop_hook(GB *gb) {
  BASE(minimapPopupType_shop);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x0c;
  CYC(b_+2, b_+5); A = mem_rd(gb, wTmpcbb6);
  CYC(b_+5, b_+7); alu_cp(gb, 0x5e);
  if (F & FZ) { CYCT(b_+7, b_+9); goto gotPopup; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+12); alu_cp(gb, 0xe8);
  if (F & FZ) { CYCT(b_+12, b_+14); goto gotPopup; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
gotPopup:
  CYC(b_+15, b_+16); A = E;
  RET(b_+16); return;
}
