#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(tryToBreakTile_body), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(tryToBreakTile_body), (from), (to), true)

#define breakableTileCollisionTable_bank06 SYM(breakableTileCollisionTable)
#define breakableTileModes_bank06 SYM(breakableTileModes)

void makeInteractionForBreakableTile_hook(GB *gb);

static void breakable_tiles_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void breakable_tiles_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void tryToBreakTile_deleteSomariaBlock(GB *gb) {
  BASE(tryToBreakTile_body);
  CYC(b_+202, b_+204); L = 0x04;
  CYC(b_+204, b_+205); A = mem_rd(gb, HL);
  CYC(b_+205, b_+207); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+207, b_+208); ret_effect(gb); return;
  }
  CYC(b_+207, b_+208);
  CYC(b_+208, b_+210); L = 0x2f;
  CYC(b_+210, b_+212); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5));
  CYC(b_+212, SYM(itemMakeInteractionForBreakableTile)); ret_effect(gb);
}

void tryToBreakTile_body_hook(GB *gb) {
  BASE(tryToBreakTile_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_and(gb, 0xf0);
  CYC(b_+3, b_+5); alu_or(gb, 0x08);
  CYC(b_+5, b_+7); H8(hFF90) = A;
  CYC(b_+7, b_+8); A = C;
  CYC(b_+8, b_+10); alu_and(gb, 0xf0);
  CYC(b_+10, b_+12); alu_or(gb, 0x08);
  CYC(b_+12, b_+14); H8(hFF91) = A;
  CALL_C(b_+14, getTileAtPosition_hook, SYM(getTileAtPosition), b_+17);
  CYC(b_+17, b_+19); H8(hFF92) = A;
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+21); A = L;
  CYC(b_+21, b_+23); H8(hFF93) = A;
  CYC(b_+23, b_+26); SET_HL(breakableTileCollisionTable_bank06);
  CALL_C(b_+26, lookupCollisionTable_paramE_hook, SYM(lookupCollisionTable_paramE), b_+29);
  if (!(F & FC)) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); E = A;
  CYC(b_+31, b_+32); alu_add(gb, A);
  CYC(b_+32, b_+35); SET_HL(breakableTileModes_bank06);
  CYC(b_+35, b_+36); breakable_tiles_add_double_index(gb, b_+36);
  CYC(b_+36, b_+37); A = E;
  CYC(b_+37, b_+38); push_effect(gb, b_+38); breakable_tiles_add_a_to_hl(gb);
  CYC(b_+38, b_+40); A = H8(hFF8F);
  CYC(b_+40, b_+41); E = A;
  CYC(b_+41, b_+43); alu_and(gb, 0x1f);
  CALL_C(b_+43, checkFlag_hook, SYM(checkFlag), b_+46);
  if (F & FZ) {
    CYCT(b_+46, b_+47); ret_effect(gb); return;
  }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); E = alu_rl(gb, E);
  if (F & FC) {
    CYCT(b_+49, b_+50); ret_effect(gb); return;
  }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+51); SET_HL(HL + 1);
  CYC(b_+51, b_+52); SET_HL(HL + 1);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+55); A = alu_swap(gb, A);
  CYC(b_+55, b_+57); alu_and(gb, 0x0f);
  CYC(b_+57, b_+59); H8(hFF8D) = A;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+60, b_+62); H8(hFF8E) = A;
  CYC(b_+62, b_+63); push_effect(gb, DE);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+65, b_+67);
    goto doneSettingTile;
  }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); A = H8(hFF92);
  CYC(b_+69, b_+71); alu_cp(gb, 0xdb);
  if (F & FZ) {
    CYCT(b_+71, b_+73);
    goto useOriginalLayout;
  }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); A = W8(wActiveCollisions);
  CYC(b_+76, b_+78); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+78, b_+80);
    goto activeCollisions1Or2;
  }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+82, b_+84);
    goto useGivenValue;
  }
  CYC(b_+82, b_+84);
activeCollisions1Or2:
  CYC(b_+84, b_+86); A = H8(hFF92);
  CYC(b_+86, b_+88); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+88, b_+90);
    goto useGivenValue;
  }
  CYC(b_+88, b_+90);
useOriginalLayout:
  CYC(b_+90, b_+92); A = H8(hFF93);
  CYC(b_+92, b_+93); push_effect(gb, HL);
  CALL_C(b_+93, getTileIndexFromRoomLayoutBuffer_hook, SYM(getTileIndexFromRoomLayoutBuffer), b_+96);
  CYC(b_+96, b_+97); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+97, b_+99);
    goto setTile;
  }
  CYC(b_+97, b_+99);
useGivenValue:
  CYC(b_+99, b_+101); A = H8(hFF93);
  CYC(b_+101, b_+102); C = A;
  CYC(b_+102, b_+103); B = mem_rd(gb, HL);
  CALL_C(b_+103, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+106);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
setTile:
  CALL_C(b_+107, setTile_hook, SYM(setTile), b_+110);
doneSettingTile:
  CYC(b_+110, b_+112); A = H8(hFF92);
  CYC(b_+112, b_+114); alu_cp(gb, 0xda);
  if (F & FZ) {
    CYCT(b_+114, b_+116);
    goto somariaBlock;
  }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+118); alu_cp(gb, 0xf2);
  CYC(b_+118, b_+121); SET_HL(wTotalSignsDestroyed);
  if (F & FZ) CALL_C_CC(b_+121, incHlRefWithCap_hook, SYM(incHlRefWithCap), b_+124);
  else CYC(b_+121, b_+124);
  CYC(b_+124, b_+126); A = H8(hFF8E);
  CYC(b_+126, b_+127); alu_rlca(gb);
  CYC(b_+127, b_+129); A = H8(hFF92);
  if (F & FC) CALL_C_CC(b_+129, updateRoomFlagsForBrokenTile_hook, SYM(updateRoomFlagsForBrokenTile), b_+132);
  else CYC(b_+129, b_+132);
  CYC(b_+132, b_+134); A = H8(hFF8E);
  CYC(b_+134, b_+136); alu_bit(gb, 6, A);
  CYC(b_+136, b_+138); A = 0x4d;
  if (!(F & FZ)) CALL_C_CC(b_+138, playSound_b00_hook, SYM(playSound_b00), b_+141);
  else CYC(b_+138, b_+141);
  CYC(b_+141, b_+144); SET_HL(wccaa);
  CYC(b_+144, b_+146); A = H8(hFF93);
  CYC(b_+146, b_+147); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+147, b_+149);
    CYC(b_+153, b_+155); A = H8(hFF8D);
    CYC(b_+155, b_+156); alu_or(gb, A);
    if (!(F & FZ)) CALL_C_CC(b_+156, decideItemDropForBrokenTile_hook, SYM(decideItemDropForBrokenTile), b_+159);
    else CYC(b_+156, b_+159);
  } else {
    CYC(b_+147, b_+149);
    CYC(b_+149, b_+151); mem_wr(gb, HL, 0xff);
    CYC(b_+151, b_+153);
  }
  CYC(b_+159, b_+161); A = H8(hFF8F);
  CYC(b_+161, b_+162); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+162, b_+164);
    goto done;
  }
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+166, b_+168);
    goto done;
  }
  CYC(b_+166, b_+168);
  CYC(b_+168, b_+170); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+170, b_+172);
    goto done;
  }
  CYC(b_+170, b_+172);
  CYC(b_+172, b_+174); alu_cp(gb, 0x12);
  CYC(b_+174, b_+176); A = H8(hFF8E);
  if (!(F & FZ)) CALL_C_CC(b_+176, makeInteractionForBreakableTile_hook, SYM(makeInteractionForBreakableTile), b_+179);
  else CYC(b_+176, b_+179);
done:
  CYC(b_+179, b_+180); SET_DE(pop_effect(gb));
  CYC(b_+180, b_+181); alu_scf(gb);
  CYC(b_+181, b_+182); ret_effect(gb); return;
somariaBlock:
  CYC(b_+182, b_+184); C = 0x18;
  CALL_C(b_+184, findItemWithID_hook, SYM(findItemWithID), b_+187);
  if (!(F & FZ)) {
    CYCT(b_+187, b_+189);
    goto done;
  }
  CYC(b_+187, b_+189);
  CYC(b_+189, b_+192); push_effect(gb, b_+192); tryToBreakTile_deleteSomariaBlock(gb);
  CALL_C(b_+192, findItemWithID_startingAfterH_hook, SYM(findItemWithID_startingAfterH), b_+195);
  if (!(F & FZ)) {
    CYCT(b_+195, b_+197);
    goto done;
  }
  CYC(b_+195, b_+197);
  CYC(b_+197, b_+200); push_effect(gb, b_+200); tryToBreakTile_deleteSomariaBlock(gb);
  CYC(b_+200, b_+202);
  goto done;
}

void itemMakeInteractionForBreakableTile_hook(GB *gb) {
  BASE(itemMakeInteractionForBreakableTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0b;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); H8(hFF90) = A;
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+10); H8(hFF91) = A;
  CYC(b_+10, b_+12); L = 0x03;
  CYC(b_+12, SYM(makeInteractionForBreakableTile)); A = mem_rd(gb, HL);
  makeInteractionForBreakableTile_hook(gb);
}

void makeInteractionForBreakableTile_hook(GB *gb) {
  BASE(makeInteractionForBreakableTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_and(gb, 0x1f);
  CYC(b_+2, b_+4); alu_cp(gb, 0x1f);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); C = A;
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = C;
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_and(gb, 0x10);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+23); A = W8(w1Link_direction);
  CYC(b_+23, b_+25); L = 0x48;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); alu_rrca(gb);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); A = H8(hFF90);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+37); A = H8(hFF91);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, SYM(decideItemDropForBrokenTile)); ret_effect(gb);
}

void decideItemDropForBrokenTile_hook(GB *gb) {
  BASE(decideItemDropForBrokenTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CALL_C(b_+1, decideItemDrop_hook, SYM(decideItemDrop), b_+4);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto done;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, getFreePartSlot_hook, SYM(getFreePartSlot), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    goto done;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x01);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); mem_wr(gb, HL, C);
  CYC(b_+15, b_+17); L = 0xcb;
  CYC(b_+17, b_+19); A = H8(hFF90);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); A = H8(hFF91);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); A = W8(w1Link_direction);
  CYC(b_+27, b_+29); A = alu_swap(gb, A);
  CYC(b_+29, b_+30); alu_rrca(gb);
  CYC(b_+30, b_+32); L = 0xc9;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+35); L = 0xc3;
  CYC(b_+35, b_+36); A = C;
  CYC(b_+36, b_+38); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+40);
  } else {
    CYC(b_+38, b_+40);
    CYC(b_+40, b_+42); mem_wr(gb, HL, 0x02);
  }
  CYC(b_+42, b_+44); A = H8(hFF8F);
  CYC(b_+44, b_+46); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+48);
  } else {
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
done:
  CYC(b_+49, b_+50); SET_HL(pop_effect(gb));
  CYC(b_+50, SYM(functionCaller_b06)); ret_effect(gb);
}
