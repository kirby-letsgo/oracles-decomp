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
  CALL_C(b_+6, s_checkIsTempleRemains, SYM(checkIsTempleRemains), b_+9);
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
