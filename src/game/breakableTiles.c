#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  AGES_ONLY();
  CYC(b_+202, b_+204); L = 0x04;
  CYC(b_+204, b_+205); A = mem_rd(gb, HL);
  CYC(b_+205, b_+207); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+207, b_+208); ret_effect(gb); return;
  }
  CYC(b_+207, b_+208);
  CYC(b_+208, b_+210); L = 0x2f;
  CYC(b_+210, b_+212); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5));
  CYC(b_+212, b_+213); ret_effect(gb);
}

void tryToBreakTile_body_hook(GB *gb) {
  BASE(tryToBreakTile_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(1)); A = B;
  CYC(b_+O(1), b_+OE(3)); alu_and(gb, 0xf0);
  CYC(b_+O(3), b_+OE(5)); alu_or(gb, 0x08);
  CYC(b_+O(5), b_+OE(7)); H8(hFF90) = A;
  CYC(b_+O(7), b_+OE(8)); A = C;
  CYC(b_+O(8), b_+OE(10)); alu_and(gb, 0xf0);
  CYC(b_+O(10), b_+OE(12)); alu_or(gb, 0x08);
  CYC(b_+O(12), b_+OE(14)); H8(hFF91) = A;
  CALL_C(b_+O(14), getTileAtPosition_hook, SYM(getTileAtPosition), b_+OE(17));
  CYC(b_+O(17), b_+OE(19)); H8(hFF92) = A;
  CYC(b_+O(19), b_+OE(20)); E = A;
  CYC(b_+O(20), b_+OE(21)); A = L;
  CYC(b_+O(21), b_+OE(23)); H8(hFF93) = A;
  CYC(b_+O(23), b_+OE(26)); SET_HL(breakableTileCollisionTable_bank06);
  CALL_C(b_+O(26), lookupCollisionTable_paramE_hook, SYM(lookupCollisionTable_paramE), b_+OE(29));
  if (!(F & FC)) {
    CYCT(b_+O(29), b_+OE(30)); ret_effect(gb); return;
  }
  CYC(b_+O(29), b_+OE(30));
  CYC(b_+O(30), b_+OE(31)); E = A;
  CYC(b_+O(31), b_+OE(32)); alu_add(gb, A);
  CYC(b_+O(32), b_+OE(35)); SET_HL(breakableTileModes_bank06);
  CYC(b_+O(35), b_+OE(36)); breakable_tiles_add_double_index(gb, b_+O(36));
  CYC(b_+O(36), b_+OE(37)); A = E;
  CYC(b_+O(37), b_+OE(38)); push_effect(gb, b_+OE(38)); breakable_tiles_add_a_to_hl(gb);
  CYC(b_+O(38), b_+OE(40)); A = H8(hFF8F);
  CYC(b_+O(40), b_+OE(41)); E = A;
  CYC(b_+O(41), b_+OE(43)); alu_and(gb, 0x1f);
  CALL_C(b_+O(43), checkFlag_hook, SYM(checkFlag), b_+OE(46));
  if (F & FZ) {
    CYCT(b_+O(46), b_+OE(47)); ret_effect(gb); return;
  }
  CYC(b_+O(46), b_+OE(47));
  CYC(b_+O(47), b_+OE(49)); E = alu_rl(gb, E);
  if (F & FC) {
    CYCT(b_+O(49), b_+OE(50)); ret_effect(gb); return;
  }
  CYC(b_+O(49), b_+OE(50));
  CYC(b_+O(50), b_+OE(51)); SET_HL(HL + 1);
  CYC(b_+O(51), b_+OE(52)); SET_HL(HL + 1);
  CYC(b_+O(52), b_+OE(53)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(53), b_+OE(55)); A = alu_swap(gb, A);
  CYC(b_+O(55), b_+OE(57)); alu_and(gb, 0x0f);
  CYC(b_+O(57), b_+OE(59)); H8(hFF8D) = A;
  CYC(b_+O(59), b_+OE(60)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(60), b_+OE(62)); H8(hFF8E) = A;
  CYC(b_+O(62), b_+OE(63)); push_effect(gb, DE);
  CYC(b_+O(63), b_+OE(64)); A = mem_rd(gb, HL);
  CYC(b_+O(64), b_+OE(65)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(65), b_+OE(67));
    goto doneSettingTile;
  }
  CYC(b_+O(65), b_+OE(67));
  if (game_seasons) {
    CYC(b_+S(67), b_+S(70)); A = W8(wActiveGroup);
    CYC(b_+S(70), b_+S(72)); alu_cp(gb, 0x03);
    if (F & FC) { CYCT(b_+S(72), b_+S(74)); goto useGivenValue; }
    CYC(b_+S(72), b_+S(74));
  } else {
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
  }
activeCollisions1Or2:
  CYC(b_+O(84), b_+OE(86)); A = H8(hFF92);
  CYC(b_+O(86), b_+OE(88)); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+O(88), b_+OE(90));
    goto useGivenValue;
  }
  CYC(b_+O(88), b_+OE(90));
useOriginalLayout:
  CYC(b_+O(90), b_+OE(92)); A = H8(hFF93);
  CYC(b_+O(92), b_+OE(93)); push_effect(gb, HL);
  CALL_C(b_+O(93), getTileIndexFromRoomLayoutBuffer_hook, SYM(getTileIndexFromRoomLayoutBuffer), b_+OE(96));
  CYC(b_+O(96), b_+OE(97)); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+O(97), b_+OE(99));
    goto setTile;
  }
  CYC(b_+O(97), b_+OE(99));
useGivenValue:
  CYC(b_+O(99), b_+OE(101)); A = H8(hFF93);
  CYC(b_+O(101), b_+OE(102)); C = A;
  CYC(b_+O(102), b_+OE(103)); B = mem_rd(gb, HL);
  CALL_C(b_+O(103), setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+OE(106));
  CYC(b_+O(106), b_+OE(107)); A = mem_rd(gb, HL);
setTile:
  CALL_C(b_+O(107), setTile_hook, SYM(setTile), b_+OE(110));
doneSettingTile:
  CYC(b_+O(110), b_+OE(112)); A = H8(hFF92);
  if (!game_seasons) {
    CYC(b_+112, b_+114); alu_cp(gb, 0xda);
    if (F & FZ) {
      CYCT(b_+114, b_+116);
      goto somariaBlock;
    }
    CYC(b_+114, b_+116);
  }
  CYC(b_+O(116), b_+OE(118)); alu_cp(gb, 0xf2);
  CYC(b_+O(118), b_+OE(121)); SET_HL(wTotalSignsDestroyed);
  if (F & FZ) CALL_C_CC(b_+O(121), incHlRefWithCap_hook, SYM(incHlRefWithCap), b_+OE(124));
  else CYC(b_+O(121), b_+OE(124));
  CYC(b_+O(124), b_+OE(126)); A = H8(hFF8E);
  CYC(b_+O(126), b_+OE(127)); alu_rlca(gb);
  CYC(b_+O(127), b_+OE(129)); A = H8(hFF92);
  if (F & FC) CALL_C_CC(b_+O(129), updateRoomFlagsForBrokenTile_hook, SYM(updateRoomFlagsForBrokenTile), b_+OE(132));
  else CYC(b_+O(129), b_+OE(132));
  CYC(b_+O(132), b_+OE(134)); A = H8(hFF8E);
  CYC(b_+O(134), b_+OE(136)); alu_bit(gb, 6, A);
  CYC(b_+O(136), b_+OE(138)); A = 0x4d;
  if (!(F & FZ)) CALL_C_CC(b_+O(138), playSound_b00_hook, SYM(playSound_b00), b_+OE(141));
  else CYC(b_+O(138), b_+OE(141));
  CYC(b_+O(141), b_+OE(144)); SET_HL(wccaa);
  CYC(b_+O(144), b_+OE(146)); A = H8(hFF93);
  CYC(b_+O(146), b_+OE(147)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+O(147), b_+OE(149));
    CYC(b_+O(153), b_+OE(155)); A = H8(hFF8D);
    CYC(b_+O(155), b_+OE(156)); alu_or(gb, A);
    if (!(F & FZ)) CALL_C_CC(b_+O(156), decideItemDropForBrokenTile_hook, SYM(decideItemDropForBrokenTile), b_+OE(159));
    else CYC(b_+O(156), b_+OE(159));
  } else {
    CYC(b_+O(147), b_+OE(149));
    CYC(b_+O(149), b_+OE(151)); mem_wr(gb, HL, 0xff);
    CYC(b_+O(151), b_+OE(153));
  }
  CYC(b_+O(159), b_+OE(161)); A = H8(hFF8F);
  CYC(b_+O(161), b_+OE(162)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(162), b_+OE(164));
    goto done;
  }
  CYC(b_+O(162), b_+OE(164));
  CYC(b_+O(164), b_+OE(166)); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+O(166), b_+OE(168));
    goto done;
  }
  CYC(b_+O(166), b_+OE(168));
  CYC(b_+O(168), b_+OE(170)); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+O(170), b_+OE(172));
    goto done;
  }
  CYC(b_+O(170), b_+OE(172));
  CYC(b_+O(172), b_+OE(174)); alu_cp(gb, 0x12);
  CYC(b_+O(174), b_+OE(176)); A = H8(hFF8E);
  if (!(F & FZ)) CALL_C_CC(b_+O(176), makeInteractionForBreakableTile_hook, SYM(makeInteractionForBreakableTile), b_+OE(179));
  else CYC(b_+O(176), b_+OE(179));
done:
  CYC(b_+O(179), b_+OE(180)); SET_DE(pop_effect(gb));
  CYC(b_+O(180), b_+OE(181)); alu_scf(gb);
  CYC(b_+O(181), b_+OE(182)); ret_effect(gb); return;
  if (!game_seasons) {
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
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  TAIL(makeInteractionForBreakableTile);
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
  CYC(b_+38, b_+39); ret_effect(gb);
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
  CYC(b_+50, b_+51); ret_effect(gb);
}
