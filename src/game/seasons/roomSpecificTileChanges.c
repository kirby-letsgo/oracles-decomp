#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/roomSpecificTileChanges.s, bank $04: per-room layout patches
// applied after the room loads.

// Horon Village: on a GBC the clock shop sign shows.
void s_tileReplacement_group0Mapc5_hook(GB *gb) {
  BASE(tileReplacement_group0Mapc5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hGameboyType);
  CYC(b_+2, b_+3); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout + 0x14);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xea);
  RET(b_+9); return;
}

// Open Maku tree gates once room flag bit 7 is set.
void s_tileReplacement_group0Mapd9_hook(GB *gb) {
  BASE(tileReplacement_group0Mapd9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x14);
  CYC(b_+9, b_+11); A = 0xbf;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x24;
  CYC(b_+15, b_+17); A = 0xa9;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  RET(b_+20); return;
}

// Torch puzzle room: a lit torch once room flag bit 6 is set, otherwise water over the
// positions listed in the table (pairs of position, tile; two tiles per row).
void s_tileReplacement_group0Mape4_hook(GB *gb) {
  BASE(tileReplacement_group0Mape4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+5, b_+7); goto water; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wRoomLayout + 0x77);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xa1); // TILEINDEX_OVERWORLD_LIT_TORCH
  RET(b_+12); return;

water:
  CYC(b_+13, b_+16); SET_HL(b_+28); // @table_group0Mape4
  CYC(b_+16, b_+18); D = 0xcf;
  for (;;) {
    CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+19, b_+20); alu_or(gb, A);
    if (F & FZ) { RET_TAKEN(b_+20); return; }
    CYC(b_+20, b_+21);
    CYC(b_+21, b_+22); E = A;
    CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+23, b_+24); mem_wr(gb, DE, A);
    CYC(b_+24, b_+25); E = alu_inc8(gb, E);
    CYC(b_+25, b_+26); mem_wr(gb, DE, A);
    CYCT(b_+26, b_+28);
  }
}

static void tiles_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// A sign outside the couple's house once the game is finished.
void s_tileReplacement_group0Mapf6_hook(GB *gb) {
  BASE(tileReplacement_group0Mapf6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x28; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x33);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xf2); // TILEINDEX_SIGN
  RET(b_+11); return;
}

// Open Tarm gates once room flag bit 7 is set.
void s_tileReplacement_group0Map63_hook(GB *gb) {
  BASE(tileReplacement_group0Map63);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x14);
  CYC(b_+9, b_+11); A = 0xad;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x24;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  RET(b_+17); return;
}

// Mr. Write's house: a lit torch once room flag bit 6 is set.
void s_tileReplacement_group2_3Mapa4_hook(GB *gb) {
  BASE(tileReplacement_group2_3Mapa4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x36);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x09); // TILEINDEX_LIT_TORCH
  RET(b_+11); return;
}

// Moldorm guarding the jewel: once beaten (bit 6) the room opens up; the chest stays until
// bit 5.
void s_tileReplacement_group0Mapf4_hook(GB *gb) {
  BASE(tileReplacement_group0Mapf4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto open; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); SET_HL(wRoomLayout + 0x45);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf1); // TILEINDEX_CHEST
open:
  CYC(b_+15, b_+18); SET_HL(wRoomLayout + 0x22);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0f);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x11);
  CYC(b_+23, b_+25); L = 0x32;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x11);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0f);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x11);
  RET(b_+33); return;
}

// D4 1F: the stairs to B1 are pits until room flag bit 7.
void s_tileReplacement_group4Map78_hook(GB *gb) {
  BASE(tileReplacement_group4Map78);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x39);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xb0);
  RET(b_+11); return;
}

// D4 three-torch room: the torches are unlit again while on the minecart.
void s_tileReplacement_group4Map64_hook(GB *gb) {
  BASE(tileReplacement_group4Map64);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+12); // @tileReplaceTable
  CYC(b_+9, b_+12);
  TAIL(replaceTiles);
}

// Member's shop: with every item bought, the chest gambling game opens.
void s_tileReplacement_group2_3Mapb0_hook(GB *gb) {
  BASE(tileReplacement_group2_3Mapb0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wBoughtShopItems1);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+28); // @rect
  CALL_C(b_+11, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+14);
  CYC(b_+14, b_+16); A = 0xf1; // TILEINDEX_CHEST
  CYC(b_+16, b_+19); SET_HL(wRoomLayout + 0x25);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = 0x27;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); L = 0x32;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xa0); // TILEINDEX_STANDARD_FLOOR
  RET(b_+27); return;
}

// D2 hidden rupee room.
void s_tileReplacement_group4Map2e_hook(GB *gb) {
  BASE(tileReplacement_group4Map2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRoomLayout + 0x23);
  CYC(b_+3, b_+6); SET_BC(0x0808);
  CYC(b_+6, b_+9); SET_DE(wGroup1RoomFlags + 0xf0); // wD2RupeeRoomRupees
  CYC(b_+9, b_+12);
  TAIL_S(replaceRupeeRoomRupees);
}

// D6 hidden rupee room.
void s_tileReplacement_group4Mapbb_hook(GB *gb) {
  BASE(tileReplacement_group4Mapbb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRoomLayout + 0x34);
  CYC(b_+3, b_+6); SET_BC(0x0808);
  CYC(b_+6, b_+9); SET_DE(wGroup1RoomFlags + 0xf8); // wD6RupeeRoomRupees
  CYC(b_+9, b_+12);
  TAIL_S(replaceRupeeRoomRupees);
}

// D5: the magnet glove chest, opened (bit 5), becomes floor.
void s_tileReplacement_group4Map89_hook(GB *gb) {
  BASE(tileReplacement_group4Map89);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+12); // @tileReplaceTable
  CYC(b_+9, b_+12);
  TAIL(replaceTiles);
}

// D8 huge lava pool, top-left screen: this room's bit 6 floods the left, the next room's
// the bottom right.
void s_tileReplacement_group5Map65_hook(GB *gb) {
  BASE(tileReplacement_group5Map65);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto next; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_d8LavaRoomsReplaceLavaSpewingFace_hook, SYM(d8LavaRoomsReplaceLavaSpewingFace), b_+10);
  CYC(b_+10, b_+13); SET_HL(b_+29); // @leftLava
  CALL_C(b_+13, s_d8LavaRoomsFillTilesWithLava_hook, SYM(d8LavaRoomsFillTilesWithLava), b_+16);
next:
  CALL_C(b_+16, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+19);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+22); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); SET_HL(b_+81); // @bottomRightLava
  CYC(b_+26, b_+29);
  TAIL_S(d8LavaRoomsFillTilesWithLava);
}

// D8 huge lava pool, center-left screen: flooded by rooms $65, $66 and $6a.
void s_tileReplacement_group5Map67_hook(GB *gb) {
  BASE(tileReplacement_group5Map67);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x65; // ROOM_SEASONS_565
  CALL_C(b_+2, getARoomFlags_hook, SYM(getARoomFlags), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+7, b_+9); goto right; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(b_+44); // @topLeftLava
  CALL_C(b_+12, s_d8LavaRoomsFillTilesWithLava_hook, SYM(d8LavaRoomsFillTilesWithLava), b_+15);
right:
  CYC(b_+15, b_+17); A = 0x66; // ROOM_SEASONS_566
  CALL_C(b_+17, getARoomFlags_hook, SYM(getARoomFlags), b_+20);
  CYC(b_+20, b_+22); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+22, b_+24); goto bottom; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); SET_HL(b_+77); // @topRightLava
  CALL_C(b_+27, s_d8LavaRoomsFillTilesWithLava_hook, SYM(d8LavaRoomsFillTilesWithLava), b_+30);
bottom:
  CYC(b_+30, b_+32); A = 0x6a; // ROOM_SEASONS_56a
  CALL_C(b_+32, getARoomFlags_hook, SYM(getARoomFlags), b_+35);
  CYC(b_+35, b_+37); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); SET_HL(b_+96); // @bottomLava
  CYC(b_+41, b_+44);
  TAIL_S(d8LavaRoomsFillTilesWithLava);
}

// D8 huge lava pool, center-right screen: room $66 floods the top half, $6b the bottom.
void s_tileReplacement_group5Map68_hook(GB *gb) {
  BASE(tileReplacement_group5Map68);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x66; // ROOM_SEASONS_566
  CALL_C(b_+2, getARoomFlags_hook, SYM(getARoomFlags), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+7, b_+9); goto bottom; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(wRoomLayout);
  CYC(b_+12, b_+14); B = 0x70;
  CALL_C(b_+14, s_replaceAllLavaTilesInGivenRange_hook, SYM(replaceAllLavaTilesInGivenRange), b_+17);
bottom:
  CYC(b_+17, b_+19); A = 0x6b; // ROOM_SEASONS_56b
  CALL_C(b_+19, getARoomFlags_hook, SYM(getARoomFlags), b_+22);
  CYC(b_+22, b_+24); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); SET_HL(wRoomLayout + 0x70);
  CYC(b_+28, b_+30); B = 0x00;
  TAIL_S(replaceAllLavaTilesInGivenRange);
}

// Replaces the five dungeon lava tiles ($61..$65) with $d7 from hl up to position b.
void s_replaceAllLavaTilesInGivenRange_hook(GB *gb) {
  BASE(replaceAllLavaTilesInGivenRange);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL);
    CYC(b_+1, b_+3); alu_sub(gb, 0x61); // TILEINDEX_DUNGEON_LAVA_1
    CYC(b_+3, b_+5); alu_cp(gb, 0x05);
    if (!(F & FC)) { CYCT(b_+5, b_+7); goto next; }
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+9); mem_wr(gb, HL, 0xd7);
next:
    CYC(b_+9, b_+10); L = alu_inc8(gb, L);
    CYC(b_+10, b_+11); A = L;
    CYC(b_+11, b_+12); alu_cp(gb, B);
    if (!(F & FZ)) { CYCT(b_+12, b_+14); continue; }
    CYC(b_+12, b_+14);
    break;
  }
  RET(b_+14); return;
}

// D8 huge lava pool, top-right and bottom screens (also group5Map6a/6b): once flooded, the
// face is shut and every lava tile becomes $d7.
void s_tileReplacement_group5Map66_hook(GB *gb) {
  BASE(tileReplacement_group5Map66);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_d8LavaRoomsReplaceLavaSpewingFace_hook, SYM(d8LavaRoomsReplaceLavaSpewingFace), b_+9);
  CYC(b_+9, b_+12); SET_DE(b_+15); // @tileReplaceTable
  CYC(b_+12, b_+15);
  TAIL(replaceTiles);
}

// Shuts the lava-spewing face and its waterfall.
void s_d8LavaRoomsReplaceLavaSpewingFace_hook(GB *gb) {
  BASE(d8LavaRoomsReplaceLavaSpewingFace);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(b_+6); // @tileReplacetable
  CYC(b_+3, b_+6);
  TAIL(replaceTiles);
}

// D8 ice-block puzzle room, finished.
void s_tileReplacement_group5Map86_hook(GB *gb) {
  BASE(tileReplacement_group5Map86);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+25); // @tileReplaceTable
  CALL_C(b_+9, replaceTiles_hook, SYM(replaceTiles), b_+12);
  CYC(b_+12, b_+15); SET_HL(wRoomLayout + 0x4d);
  CYC(b_+15, b_+17); A = 0x2f;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = 0x5d;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = 0x6d;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  RET(b_+24); return;
}

// Holly's house: the door opens in winter.
void s_tileReplacement_group0Map7f_hook(GB *gb) {
  BASE(tileReplacement_group0Map7f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03); // SEASON_WINTER
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x47);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xea);
  RET(b_+11); return;
}

// Outside the floodgate keeper's house: water until the floodgate (Subrosia room $b5 bit 6).
void s_tileReplacement_group0Map62_hook(GB *gb) {
  BASE(tileReplacement_group0Map62);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H = 0xc8; // wSubrosiaRoomFlags
  CYC(b_+2, b_+4); L = 0xb5; // ROOM_SEASONS_2b5
  CYC(b_+4, b_+6); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_HL(b_+19); // @rect
  CALL_C(b_+10, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+13);
  CYC(b_+13, b_+16); SET_HL(wRoomLayout + 0x27);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xfd); // TILEINDEX_WATER
  RET(b_+18); return;
}

// Inside the floodgate keeper's house: a puddle until room flag bit 6.
void s_tileReplacement_group2_3Mapb5_hook(GB *gb) {
  BASE(tileReplacement_group2_3Mapb5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x37);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xfa); // TILEINDEX_PUDDLE
  RET(b_+11); return;
}

// D3 entrance screen: still flooded until room $81's bit 7.
void s_tileReplacement_group0Map60_hook(GB *gb) {
  BASE(tileReplacement_group0Map60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x81; // ROOM_SEASONS_081
  CALL_C(b_+2, getARoomFlags_hook, SYM(getARoomFlags), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+20); // @rect1
  CALL_C(b_+11, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+14);
  CYC(b_+14, b_+17); SET_HL(b_+24); // @rect2
  CYC(b_+17, b_+20);
  TAIL(fillRectInRoomLayout);
}

// Screen below the D3 entrance.
void s_tileReplacement_group0Map70_hook(GB *gb) {
  BASE(tileReplacement_group0Map70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x81; // ROOM_SEASONS_081
  CALL_C(b_+2, getARoomFlags_hook, SYM(getARoomFlags), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+14); // @rect
  CYC(b_+11, b_+14);
  TAIL(fillRectInRoomLayout);
}

// Spool Swamp screen with Sokra.
void s_tileReplacement_group0Map71_hook(GB *gb) {
  BASE(tileReplacement_group0Map71);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x81; // ROOM_SEASONS_081
  CALL_C(b_+2, getARoomFlags_hook, SYM(getARoomFlags), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+20); // @rect1
  CALL_C(b_+11, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+14);
  CYC(b_+14, b_+17); SET_HL(b_+24); // @rect2
  CYC(b_+17, b_+20);
  TAIL(fillRectInRoomLayout);
}

// Spool Swamp keylock screen: water until drained, then a puddle (dry ground in winter).
void s_tileReplacement_group0Map81_hook(GB *gb) {
  BASE(tileReplacement_group0Map81);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto drained; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(b_+29); // @rect1
  CYC(b_+10, b_+13);
  TAIL(fillRectInRoomLayout);
drained:
  CYC(b_+13, b_+16); SET_HL(b_+37); // @rect3
  CYC(b_+16, b_+19); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+19, b_+21); alu_cp(gb, 0x03); // SEASON_WINTER
  if (F & FZ) { CYCT(b_+21, b_+23); goto fill; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); SET_HL(b_+33); // @rect2
fill:
  CYC(b_+26, b_+29);
  TAIL(fillRectInRoomLayout);
}

// Screen above the D4 entrance: the waterfall until room flag bit 7.
void s_tileReplacement_group0Map0d_hook(GB *gb) {
  BASE(tileReplacement_group0Map0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+12); // @rect
  CYC(b_+9, b_+12);
  TAIL(fillRectInRoomLayout);
}

// D4 entrance screen: the waterfall until room flag bit 7.
void s_tileReplacement_group0Map1d_hook(GB *gb) {
  BASE(tileReplacement_group0Map1d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+23); // @rect
  CALL_C(b_+9, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+12);
  CYC(b_+12, b_+14); L = 0x13;
  CYC(b_+14, b_+16); A = 0xfe; // TILEINDEX_WATERFALL_BOTTOM
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = 0x22;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  RET(b_+22); return;
}

// Fills the rect at bc once room flag bit 7 is set.
void s_fillRectIfRoomFlagBit7Set_hook(GB *gb) {
  BASE(fillRectIfRoomFlagBit7Set);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = B;
  CYC(b_+7, b_+8); L = C;
  CYC(b_+8, b_+11);
  TAIL(fillRectInRoomLayout);
}

// D4 B2 torch room bridge.
void s_tileReplacement_group4Map61_hook(GB *gb) {
  BASE(tileReplacement_group4Map61);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+5); // @rect
  CYCT(b_+3, b_+5);
  TAIL_S(fillRectIfRoomFlagBit7Set);
}

// D7 darknut room bridge.
void s_tileReplacement_group5Map3b_hook(GB *gb) {
  BASE(tileReplacement_group5Map3b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+5); // @rect
  CYCT(b_+3, b_+5);
  TAIL_S(fillRectIfRoomFlagBit7Set);
}

// D8 top-right long bridge.
void s_tileReplacement_group5Map7a_hook(GB *gb) {
  BASE(tileReplacement_group5Map7a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+5); // @rect
  CYCT(b_+3, b_+5);
  TAIL_S(fillRectIfRoomFlagBit7Set);
}

// D8 HSS-skip bridge.
void s_tileReplacement_group5Map78_hook(GB *gb) {
  BASE(tileReplacement_group5Map78);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+5); // @rect
  CYCT(b_+3, b_+5);
  TAIL_S(fillRectIfRoomFlagBit7Set);
}

// D8 1F bridge into the lava.
void s_tileReplacement_group5Map8e_hook(GB *gb) {
  BASE(tileReplacement_group5Map8e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+5); // @rect
  CYCT(b_+3, b_+5);
  TAIL_S(fillRectIfRoomFlagBit7Set);
}

// King Moblin's house: destroyed (3x3 rubble) until the seed tree refill marks it repaired,
// then ruined.
void s_tileReplacement_group0Map6f_hook(GB *gb) {
  BASE(tileReplacement_group0Map6f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0xc0);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+63); // @ruinedHouse
  CYC(b_+9, b_+11); A = 0x12; // GLOBALFLAG_DESTROYED_MOBLIN_HOUSE_REPAIRED
  CALL_C(b_+11, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+14);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto draw; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wSeedTreeRefilledBitset);
  CYC(b_+19, b_+21); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto repaired; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); SET_DE(b_+54); // @destroyedHouse
  CYCT(b_+26, b_+28); goto draw;
repaired:
  CYC(b_+28, b_+30); A = 0x12; // GLOBALFLAG_DESTROYED_MOBLIN_HOUSE_REPAIRED
  CALL_C(b_+30, setGlobalFlag_hook, SYM(setGlobalFlag), b_+33);
draw:
  CYC(b_+33, b_+36); SET_HL(wRoomLayout + 0x36);
  CYC(b_+36, b_+38); B = 0x03;
  for (;;) {
    CYC(b_+38, b_+40); C = 0x03;
    for (;;) {
      CYC(b_+40, b_+41); A = mem_rd(gb, DE);
      CYC(b_+41, b_+42); SET_DE(DE + 1);
      CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+43, b_+44); C = alu_dec8(gb, C);
      if (!(F & FZ)) { CYCT(b_+44, b_+46); continue; }
      CYC(b_+44, b_+46);
      break;
    }
    CYC(b_+46, b_+48); A = 0x0d;
    CYC(b_+48, b_+49); alu_add(gb, L);
    CYC(b_+49, b_+50); L = A;
    CYC(b_+50, b_+51); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+51, b_+53); continue; }
    CYC(b_+51, b_+53);
    break;
  }
  RET(b_+53); return;
}

// Tarm Ruins: the two statues, pushed into the water (room flag bits 7 and 6) or not.
void s_tileReplacement_group0Map42_hook(GB *gb) {
  BASE(tileReplacement_group0Map42);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRoomLayout + 0x44);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x9c);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+10, b_+12); goto first_dry; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); SET_HL(wRoomLayout + 0x55);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0xbc);
  CYCT(b_+17, b_+19); goto second;
first_dry:
  CYC(b_+19, b_+22); SET_HL(wRoomLayout + 0x54);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0xd6);
second:
  CALL_C(b_+24, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+27);
  CYC(b_+27, b_+29); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+29, b_+31); goto second_dry; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); SET_HL(wRoomLayout + 0x65);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0xbc);
  RET(b_+36); return;
second_dry:
  CYC(b_+37, b_+40); SET_HL(wRoomLayout + 0x64);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0xd6);
  RET(b_+42); return;
}

// The pirate door into the Samasa desert: opened (bit 7) it is two rows of four tiles.
void s_tileReplacement_group0Mapfc_hook(GB *gb) {
  BASE(tileReplacement_group0Mapfc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x03);
  CYC(b_+9, b_+11); A = 0xaf;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); A = 0x0d;
  CYC(b_+17, b_+18); tiles_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+20); A = 0xaf;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  RET(b_+24); return;
}

void s_checkPirateShipDocked_hook(GB *gb) {
  BASE(checkPirateShipDocked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17; // GLOBALFLAG_PIRATE_SHIP_DOCKED
  CYC(b_+2, b_+5);
  TAIL(checkGlobalFlag);
}

// Top screen of the pirate ship in Subrosia.
void s_tileReplacement_group1Map64_hook(GB *gb) {
  BASE(tileReplacement_group1Map64);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkPirateShipDocked_hook, SYM(checkPirateShipDocked), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(b_+10); // @rect
  CYC(b_+7, b_+10);
  TAIL(fillRectInRoomLayout);
}

// Bottom screen of the pirate ship in Subrosia.
void s_tileReplacement_group1Map74_hook(GB *gb) {
  BASE(tileReplacement_group1Map74);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkPirateShipDocked_hook, SYM(checkPirateShipDocked), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(b_+10); // @rect
  CYC(b_+7, b_+10);
  TAIL(fillRectInRoomLayout);
}

// The pirate ship screen in the Samasa desert: with the ship docked, a 5x5 quicksand patch.
void s_tileReplacement_group0Mapee_hook(GB *gb) {
  BASE(tileReplacement_group0Mapee);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkPirateShipDocked_hook, SYM(checkPirateShipDocked), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(b_+31); // @sandRect
  CYC(b_+7, b_+10); SET_DE(wRoomLayout + 0x23);
  CYC(b_+10, b_+13); SET_BC(0x0505);
  for (;;) {
    CYC(b_+13, b_+14); push_effect(gb, DE);
    CYC(b_+14, b_+15); push_effect(gb, BC);
    for (;;) {
      CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+16, b_+17); mem_wr(gb, DE, A);
      CYC(b_+17, b_+18); E = alu_inc8(gb, E);
      CYC(b_+18, b_+19); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+19, b_+21); continue; }
      CYC(b_+19, b_+21);
      break;
    }
    CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
    CYC(b_+22, b_+23); SET_DE(pop_effect(gb));
    CYC(b_+23, b_+24); A = E;
    CYC(b_+24, b_+26); alu_add(gb, 0x10);
    CYC(b_+26, b_+27); E = A;
    CYC(b_+27, b_+28); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+28, b_+30); continue; }
    CYC(b_+28, b_+30);
    break;
  }
  RET(b_+30); return;
}

// Subrosia screen with the linked locked doors: open once group 4 room $f9 has bit 2.
void s_tileReplacement_group1Map35_hook(GB *gb) {
  BASE(tileReplacement_group1Map35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGroup4RoomFlags + 0xf9);
  CYC(b_+3, b_+5); alu_and(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x43);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xe8); // TILEINDEX_OPEN_CAVE_DOOR
  RET(b_+11); return;
}

// The big bridge into Natzu (not with Dimitri): water until room flag bit 6, then the bridge
// end tile.
void s_tileReplacement_group0Map56_hook(GB *gb) {
  BASE(tileReplacement_group0Map56);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wSwitchState, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+7, b_+9); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto built; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0xfd; // TILEINDEX_WATER
  CYC(b_+19, b_+22); SET_HL(wRoomLayout + 0x43);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); SET_HL(wRoomLayout + 0x53);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  RET(b_+29); return;
built:
  CYC(b_+30, b_+32); A = 0xb0;
  CYC(b_+32, b_+35); mem_wr(gb, wRoomLayout + 0x66, A);
  RET(b_+35); return;
}

// The Moblin keep roof, removed once the keep is destroyed.
void s_tileReplacement_group0Map4b_hook(GB *gb) {
  BASE(tileReplacement_group0Map4b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x16; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x73);
  CYC(b_+9, b_+11); A = 0x40;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  RET(b_+14); return;
}

// The Twinrova/Ganon fight room: mode 1 fills it with lava, 2 with ice, 3 restores the
// layout, 4+ the seizure tiles (same as Ages).
void s_tileReplacement_group5Map9e_hook(GB *gb) {
  BASE(tileReplacement_group5Map9e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTwinrovaTileReplacementMode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto lava; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto ice; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto normal; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wTwinrovaTileReplacementMode, A);
  CYC(b_+18, b_+21); SET_HL(b_+24); // @seizureTiles
  CYC(b_+21, b_+24);
  TAIL(fillRectInRoomLayout);
normal:
  CYC(b_+28, b_+31); mem_wr(gb, wTwinrovaTileReplacementMode, A);
  CYC(b_+31, b_+33); A = 0xb9; // GFXH_TWINROVA_NORMAL_LAYOUT
  CYC(b_+33, b_+36);
  TAIL(loadGfxHeader);
ice:
  CYC(b_+36, b_+39); mem_wr(gb, wTwinrovaTileReplacementMode, A);
  CYC(b_+39, b_+42); SET_HL(b_+45); // @iceTiles
  CYC(b_+42, b_+45);
  TAIL(fillRectInRoomLayout);
lava:
  CYC(b_+49, b_+52); mem_wr(gb, wTwinrovaTileReplacementMode, A);
  CYC(b_+52, b_+54); A = 0xb8; // GFXH_TWINROVA_LAVA_LAYOUT
  CYC(b_+54, b_+57);
  TAIL(loadGfxHeader);
}

// Down the Horon Village stairs to the Subrosia portal: the bridges once room flag bit 6.
void s_tileReplacement_group2_3Mapab_hook(GB *gb) {
  BASE(tileReplacement_group2_3Mapab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x14);
  CYC(b_+9, b_+11); A = 0x6d; // TILEINDEX_HORIZONTAL_BRIDGE
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); A = 0x6a; // TILEINDEX_VERTICAL_BRIDGE
  CYC(b_+16, b_+18); L = 0x47;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = 0x37;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L = 0x27;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = 0x17;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  RET(b_+28); return;
}

// hl: the tile, then room positions up to $ff; writes the tile at each.
void s_d8LavaRoomsFillTilesWithLava_hook(GB *gb) {
  BASE(d8LavaRoomsFillTilesWithLava);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); D = 0xcf; // wRoomLayout
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  for (;;) {
    CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+5, b_+7); alu_cp(gb, 0xff);
    if (F & FZ) { RET_TAKEN(b_+7); return; }
    CYC(b_+7, b_+8);
    CYC(b_+8, b_+9); E = A;
    CYC(b_+9, b_+10); A = C;
    CYC(b_+10, b_+11); mem_wr(gb, DE, A);
    CYCT(b_+11, b_+13);
  }
}

// bc = $0808, de = the room's rupee bitset, hl = its top-left tile: every set bit's rupee
// has been taken and is floor.
void s_replaceRupeeRoomRupees_hook(GB *gb) {
  BASE(replaceRupeeRoomRupees);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, DE);
    CYC(b_+1, b_+2); SET_DE(DE + 1);
    CYC(b_+2, b_+3); push_effect(gb, BC);
    for (;;) {
      CYC(b_+3, b_+4); alu_rrca(gb);
      if (!(F & FC)) { CYCT(b_+4, b_+6); goto next; }
      CYC(b_+4, b_+6);
      CYC(b_+6, b_+8); mem_wr(gb, HL, 0xa0); // TILEINDEX_STANDARD_FLOOR
next:
      CYC(b_+8, b_+9); L = alu_inc8(gb, L);
      CYC(b_+9, b_+10); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+10, b_+12); continue; }
      CYC(b_+10, b_+12);
      break;
    }
    CYC(b_+12, b_+13); A = L;
    CYC(b_+13, b_+15); alu_add(gb, 0x08);
    CYC(b_+15, b_+16); L = A;
    CYC(b_+16, b_+17); SET_BC(pop_effect(gb));
    CYC(b_+17, b_+18); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+18, b_+20); continue; }
    CYC(b_+18, b_+20);
    break;
  }
  RET(b_+20); return;
}
