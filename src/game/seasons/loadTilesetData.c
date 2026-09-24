#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/loadTilesetData.s, bank $04: the rooms whose tileset data
// depends on the game's state rather than the season.

// getTempleRemainsSeasonsTilesetData@returnAlteredData: hl is the 8-byte entry; carry set.
static void tileset_return_altered(GB *gb) {
  BASE(getTempleRemainsSeasonsTilesetData);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+23); mem_wr(gb, hFF8B, A);
  CYC(b_+23, b_+24); alu_scf(gb);
  RET(b_+24); return;
}

// Once the Temple Remains has been altered, its tileset entry comes from templeRemainsSeasons
// by the current season.
void s_getTempleRemainsSeasonsTilesetData_hook(GB *gb) {
  BASE(getTempleRemainsSeasonsTilesetData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x15; // GLOBALFLAG_TEMPLE_REMAINS_ALTERED
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_checkIsTempleRemains_hook, SYM(checkIsTempleRemains), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wRoomStateModifier);
  CALL_C(b_+13, multiplyABy8_hook, SYM(multiplyABy8), b_+16);
  CYC(b_+16, b_+19); SET_HL(SYM(templeRemainsSeasons));
  CYC(b_+19, b_+20); alu_add_hl(gb, BC);
  tileset_return_altered(gb); return;
}

// Once the Moblin keep is destroyed, its rooms' tileset entry comes from moblinKeepSeasons by
// the companion region.
void s_getMoblinKeepSeasonsTilesetData_hook(GB *gb) {
  BASE(getMoblinKeepSeasonsTilesetData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, s_getMoblinKeepScreenIndex_hook, SYM(getMoblinKeepScreenIndex), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); A = 0x16; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+11, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+14);
  if (F & FZ) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+18, b_+20); alu_sub(gb, 0x0a);
  CYC(b_+20, b_+22); alu_and(gb, 0x03);
  CALL_C(b_+22, multiplyABy8_hook, SYM(multiplyABy8), b_+25);
  CYC(b_+25, b_+28); SET_HL(SYM(moblinKeepSeasons));
  CYC(b_+28, b_+29); alu_add_hl(gb, BC);
  CYCT(b_+29, b_+31); tileset_return_altered(gb); return;
}

// Carry set when the active room is one of the five Moblin keep screens.
void s_getMoblinKeepScreenIndex_hook(GB *gb) {
  BASE(getMoblinKeepScreenIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveRoom);
  CYC(b_+3, b_+5); B = 0x05;
  CYC(b_+5, b_+8); SET_HL(b_+19); // moblinKeepRooms
  for (;;) {
    CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+9, b_+11); goto found; }
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); SET_HL(HL + 1);
    CYC(b_+12, b_+13); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+13, b_+15); continue; }
    CYC(b_+13, b_+15);
    break;
  }
  CYC(b_+15, b_+16); alu_xor(gb, A);
  RET(b_+16); return;
found:
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
}

static void loadTilesetData_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void loadTilesetData_d_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Called from loadTilesetData in bank 0. Function differs substantially in Ages and Seasons.
void s_loadTilesetData_body_hook(GB *gb) {
  BASE(loadTilesetData_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getTempleRemainsSeasonsTilesetData_hook, SYM(getTempleRemainsSeasonsTilesetData), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); goto gotTilesetData; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_getMoblinKeepSeasonsTilesetData_hook, SYM(getMoblinKeepSeasonsTilesetData), b_+8);
  if (F & FC) { CYCT(b_+8, b_+10); goto gotTilesetData; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wActiveGroup);
  CYC(b_+13, b_+16); SET_HL(SYM(roomTilesetsGroupTable));
  CYC(b_+16, b_+17); loadTilesetData_d_add_double_index(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  CYC(b_+19, b_+20); L = A;
  CYC(b_+20, b_+23); A = mem_rd(gb, wActiveRoom);
  CYC(b_+23, b_+24); loadTilesetData_d_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_and(gb, 0x80);
  CYC(b_+27, b_+29); mem_wr(gb, hFF8B, A);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_and(gb, 0x7f);
  CALL_C(b_+32, s_multiplyABy8, SYM(multiplyABy8), b_+35);
  CYC(b_+35, b_+38); SET_HL(SYM(tilesetData));
  CYC(b_+38, b_+39); alu_add_hl(gb, BC);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto gotTilesetData; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); SET_HL(HL + 1);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); H = mem_rd(gb, HL);
  CYC(b_+46, b_+47); L = A;
  CYC(b_+47, b_+50); A = mem_rd(gb, wRoomStateModifier);
  CALL_C(b_+50, s_multiplyABy8, SYM(multiplyABy8), b_+53);
  CYC(b_+53, b_+54); alu_add_hl(gb, BC);
gotTilesetData:
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+55, b_+56); E = A;
  CYC(b_+56, b_+58); alu_and(gb, 0x0f);
  CYC(b_+58, b_+60); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto L_6cad; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); A = 0xff;
L_6cad:
  CYC(b_+64, b_+67); mem_wr(gb, wDungeonIndex, A);
  CYC(b_+67, b_+68); A = E;
  CYC(b_+68, b_+70); A = alu_swap(gb, A);
  CYC(b_+70, b_+72); alu_and(gb, 0x0f);
  CYC(b_+72, b_+75); mem_wr(gb, wActiveCollisions, A);
  CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+76, b_+79); mem_wr(gb, wTilesetFlags, A);
  CYC(b_+79, b_+81); B = 0x06;
  CYC(b_+81, b_+84); SET_DE(wTilesetUniqueGfx);
copyloop:
  CYC(b_+84, b_+85); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); E = alu_inc8(gb, E);
  CYC(b_+87, b_+88); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto copyloop; }
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92); E = 0x20;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); B = A;
  CYC(b_+94, b_+96); A = mem_rd(gb, hFF8B);
  CYC(b_+96, b_+97); alu_or(gb, B);
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+101); A = mem_rd(gb, wActiveGroup);
  CYC(b_+101, b_+102); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; }
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+106); A = mem_rd(gb, wActiveRoom);
  CYC(b_+106, b_+108); alu_cp(gb, 0x96);
  if (!(F & FZ)) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CALL_C(b_+109, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+112);
  CYC(b_+112, b_+114); alu_and(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+114); return; }
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+117); A = 0x20;
  CYC(b_+117, b_+120); mem_wr(gb, wTilesetUniqueGfx, A);
  RET(b_+120); return;
}

// @param[out]	cflag	set if active room is temple remains
void s_checkIsTempleRemains_hook(GB *gb) {
  BASE(checkIsTempleRemains);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveRoom);
  CYC(b_+8, b_+10); alu_cp(gb, 0x14);
  if (F & FC) { CYCT(b_+10, b_+12); goto L_6d15; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_sub(gb, 0x04);
  CYC(b_+14, b_+16); alu_cp(gb, 0x30);
  if (!(F & FC)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+21); alu_cp(gb, 0x04);
  RET(b_+21); return;
L_6d15:
  CYC(b_+22, b_+23); alu_xor(gb, A);
  RET(b_+23); return;
}

