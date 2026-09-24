#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/roomGfxChanges.s, bank $04: per-room graphics and collision
// patches applied after the room loads.

// Before the intro is done, load Din's troupe tiles (VRAM and collisions) from the table after
// the routine; the WRAM and ROM banks are pushed for the loader to restore.
void s_roomTileChangesAfterLoad0e_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = mem_rd(gb, IO_SVBK);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+11); A = mem_rd(gb, hRomBank);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); push_effect(gb, BC);
  CYC(b_+13, b_+16); SET_DE(b_+19); // @vramTilesAndCollisions
  CYC(b_+16, b_+19);
  TAIL_S(loadDinsTroupeTileChanges);
}

void s_roomTileChangesAfterLoad0f_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDiggingUpEnemiesForbidden, A);
  RET(b_+5); return;
}

static void roomGfxChanges_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// $09: Load scent tree graphics (north horon)
void s_roomTileChangesAfterLoad09_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x07;
  s_label_04_291_hook(gb); return; // falls through
}

void s_label_04_291_hook(GB *gb) {
  BASE(label_04_291);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_loadTreeGfx, SYM(loadTreeGfx), b_+3);
  CYC(b_+3, b_+6); SET_HL(b_+9 /* @rect */);
  CYC(b_+6, b_+9);
  TAIL(drawRectangleToVramTiles);
}

// $0a: Load pegasus tree graphics (spool swamp)
void s_roomTileChangesAfterLoad0a_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+4);
  TAIL_S(label_04_291);
}

// $0b: Load gale tree graphics (tarm ruins)
void s_roomTileChangesAfterLoad0b_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(loadGaleTreeGfx) + 8) /* @rect */);
  s_loadGaleTreeGfx_hook(gb); return; // falls through
}

void s_loadGaleTreeGfx_hook(GB *gb) {
  BASE(loadGaleTreeGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_drawRectangleToVramTiles, SYM(drawRectangleToVramTiles), b_+3);
  CYC(b_+3, b_+5); A = 0x09;
  CYC(b_+5, b_+8);
  TAIL(loadTreeGfx);
}

// $0c: Load gale tree graphics (sunken city)
void s_roomTileChangesAfterLoad0c_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+5 /* @rect */);
  CYC(b_+3, b_+5);
  TAIL_S(loadGaleTreeGfx);
}

// $0d: Load mystery tree graphics (woods of winter)
void s_roomTileChangesAfterLoad0d_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0a;
  CALL_C(b_+2, s_loadTreeGfx, SYM(loadTreeGfx), b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+11 /* @rect */);
  CYC(b_+8, b_+11);
  TAIL(drawRectangleToVramTiles);
}

// $00: Pirate ship bow (at beach)
void s_roomTileChangesAfterLoad00_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x00;
  CALL_C(b_+8, s_loadObjectGfxHeaderToSlot4, SYM(loadObjectGfxHeaderToSlot4), b_+11);
  CYC(b_+11, b_+13); A = 0x55;
  CALL_C(b_+13, s_loadGfxHeader, SYM(loadGfxHeader), b_+16);
  CYC(b_+16, b_+19); SET_HL(b_+34 /* @vramTiles */);
  CALL_C(b_+19, s_copyRectangleFromTmpGfxBuffer, SYM(copyRectangleFromTmpGfxBuffer), b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+40 /* @vramAttributes */);
  CALL_C(b_+25, s_copyRectangleFromTmpGfxBuffer, SYM(copyRectangleFromTmpGfxBuffer), b_+28);
  CYC(b_+28, b_+31); SET_HL(b_+46 /* @roomLayout */);
  CYC(b_+31, b_+34);
  TAIL(copyRectangleToRoomLayoutAndCollisions);
}

// $01: Pirate ship middle (at beach)
void s_roomTileChangesAfterLoad01_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x00;
  CALL_C(b_+8, s_loadObjectGfxHeaderToSlot4, SYM(loadObjectGfxHeaderToSlot4), b_+11);
  CYC(b_+11, b_+13); A = 0x56;
  CALL_C(b_+13, s_loadGfxHeader, SYM(loadGfxHeader), b_+16);
  CYC(b_+16, b_+19); SET_HL(b_+34 /* @vramTiles */);
  CALL_C(b_+19, s_copyRectangleFromTmpGfxBuffer, SYM(copyRectangleFromTmpGfxBuffer), b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+40 /* @vramAttributes */);
  CALL_C(b_+25, s_copyRectangleFromTmpGfxBuffer, SYM(copyRectangleFromTmpGfxBuffer), b_+28);
  CYC(b_+28, b_+31); SET_HL(b_+46 /* @roomLayout */);
  CYC(b_+31, b_+34);
  TAIL(copyRectangleToRoomLayoutAndCollisions);
}

// $03: Din's troupe screen: Draw tents and stuff if they should be there.
void s_roomTileChangesAfterLoad03_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); mem_wr(gb, wRoomLayout + 0x14, A);
  CYC(b_+10, b_+13); mem_wr(gb, wRoomLayout + 0x24, A);
  CYC(b_+13, b_+15); A = mem_rd(gb, IO_SVBK);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+18); A = mem_rd(gb, hRomBank);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); push_effect(gb, BC);
  CYC(b_+20, b_+23); SET_DE(SYM(dinsTroupeVramAndCollisions));
  s_loadDinsTroupeTileChanges_hook(gb); return; // falls through
}

void s_loadDinsTroupeTileChanges_hook(GB *gb) {
  BASE(loadDinsTroupeTileChanges);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
getAddress:
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+7); SET_DE(DE + 1);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto doneReadingVramTiles; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); H = A;
  CYC(b_+12, b_+13); SET_DE(DE + 1);
getTilePair:
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+16); SET_DE(DE + 1);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+17, b_+19); goto getAddress; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+22); SET_DE(DE + 1);
  CYC(b_+22, b_+23); push_effect(gb, HL);
  CYC(b_+23, b_+24); mem_wr(gb, HL, B);
  CYC(b_+24, b_+26); H = H | 0x04;
  CYC(b_+26, b_+27); mem_wr(gb, HL, C);
  CYC(b_+27, b_+29); A = 0x20;
  CYC(b_+29, b_+30); roomGfxChanges_d_add_a_to_hl(gb, b_+30);
  CYC(b_+30, b_+31); B = alu_inc8(gb, B);
  CYC(b_+31, b_+32); mem_wr(gb, HL, C);
  CYC(b_+32, b_+34); H = H & 0xfb;
  CYC(b_+34, b_+35); mem_wr(gb, HL, B);
  CYC(b_+35, b_+36); SET_HL(pop_effect(gb));
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+39);
  goto getTilePair;
doneReadingVramTiles:
  CYC(b_+39, b_+40); L = E;
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); D = 0xce;
nextAddress:
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+45, b_+47); goto done; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); E = A;
  CYC(b_+48, b_+50); A = 0x0f;
  CYC(b_+50, b_+52); alu_bit(gb, 7, E);
  if (F & FZ) { CYCT(b_+52, b_+54); goto L_690b; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); E = E & 0x7f;
  CYC(b_+56, b_+58); A = 0x05;
L_690b:
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+61);
  goto nextAddress;
done:
  CYC(b_+61, b_+62); SET_BC(pop_effect(gb));
  CYC(b_+62, b_+63); A = B;
  CYC(b_+63, b_+65); mem_wr(gb, hRomBank, A);
  CYC(b_+65, b_+68); mem_wr(gb, 0x2222, A);
  CYC(b_+68, b_+69); A = C;
  CYC(b_+69, b_+71); mem_wr(gb, IO_SVBK, A);
  CYC(b_+71, b_+73); A = 0x02;
  CYC(b_+73, b_+76);
  TAIL(loadTreeGfx);
}

// $05: King Moblin's house (not moblin's keep)
void s_roomTileChangesAfterLoad05_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x16;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x08;
  CALL_C(b_+8, s_loadObjectGfxHeaderToSlot4, SYM(loadObjectGfxHeaderToSlot4), b_+11);
  CYC(b_+11, b_+14); SET_HL(b_+17 /* @rect */);
  CYC(b_+14, b_+17);
  TAIL(drawRectangleToVramTiles);
}

// $06: Blaino's gym (draws gloves on roof)
void s_roomTileChangesAfterLoad06_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, s_loadTreeGfx, SYM(loadTreeGfx), b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+11 /* @rect */);
  CYC(b_+8, b_+11);
  TAIL(drawRectangleToVramTiles);
}

// $07: Vasu's shop (draws ring sign)
void s_roomTileChangesAfterLoad07_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, s_loadTreeGfx, SYM(loadTreeGfx), b_+5);
  CYC(b_+5, b_+8); SET_HL(SYM(vasuSignRect));
  CALL_C(b_+8, s_drawRectangleToVramTiles, SYM(drawRectangleToVramTiles), b_+11);
  s_roomTileChangesAfterLoad0f_hook(gb); return; // falls through
}

// $08: Gasha spot (draws the tree or the plant if something has been planted)
void s_roomTileChangesAfterLoad08_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveRoom);
  CALL_C(b_+3, s_getIndexOfGashaSpotInRoom_body, SYM(getIndexOfGashaSpotInRoom_body), b_+6);
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0xe0;
  CALL_C(b_+9, s_findTileInRoom, SYM(findTileInRoom), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+16); D = 0xcf;
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+20); SET_HL(wGashaSpotKillCounters);
  CYC(b_+20, b_+21); roomGfxChanges_d_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x14);
  if (F & FC) { CYCT(b_+24, b_+26); goto L_6a3d; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); A = E;
  CYC(b_+27, b_+29); alu_sub(gb, 0x10);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+33); SET_HL(b_+48 /* @treeLayout */);
  CYC(b_+33, b_+35);
  goto L_6a40;
L_6a3d:
  CYC(b_+35, b_+38); SET_HL(b_+44 /* @sproutLayout */);
L_6a40:
  CALL_C(b_+38, s_copyRectangleToRoomLayoutAndCollisions_paramDe, SYM(copyRectangleToRoomLayoutAndCollisions_paramDe), b_+41);
  CYC(b_+41, b_+44);
  TAIL(generateW3VramTilesAndAttributes);
}

