#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/objectLoading.s, the Seasons-only pieces.

void s_parseObjectData__afterCall58c7_hook(GB *gb);
void s_parseObjectData__afterCall58ca_hook(GB *gb);

static void parseObjectData_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Clears the enemy placement buffer, then looks the room's object data up by group and room
// (Ages uses a fixed pointer here) and falls through into parseGivenObjectData.
void s_parseObjectData_hook(GB *gb) {
  BASE(parseObjectData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wNumEnemies) = A;
  CYC(b_+4, b_+7); W8(wTmpcfc0) = A;
  CYC(b_+7, b_+10); SET_HL(wEnemyPlacement);
  CYC(b_+10, b_+12); B = 0x20;
  CALL_C(b_+12, clearMemory_hook, SYM(clearMemory), b_+15);
  CALL_C(b_+15, addRoomToEnemiesKilledList_b00_hook, SYM(addRoomToEnemiesKilledList_b00), b_+18);
  s_parseObjectData__afterCall58c7_hook(gb);
}

void s_parseObjectData__afterCall58c7_hook(GB *gb) {
  BASE(parseObjectData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+18, generateRandomBuffer_b00_hook, SYM(generateRandomBuffer_b00), b_+21);
  s_parseObjectData__afterCall58ca_hook(gb);
}

void s_parseObjectData__afterCall58ca_hook(GB *gb) {
  BASE(parseObjectData);
  CYC(b_+21, b_+24); A = mem_rd(gb, wActiveGroup);
  CYC(b_+24, b_+27); SET_HL(SYM(objectDataGroupTable));
  CYC(b_+27, b_+28); parseObjectData_addDoubleIndexToHl_from_rst(gb, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); H = mem_rd(gb, HL);
  CYC(b_+30, b_+31); L = A;
  CYC(b_+31, b_+34); A = mem_rd(gb, wActiveRoom);
  CYC(b_+34, b_+35); E = A;
  CYC(b_+35, b_+37); D = 0x00;
  CYC(b_+37, b_+38); alu_add_hl(gb, DE);
  CYC(b_+38, b_+39); alu_add_hl(gb, DE);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+41); D = mem_rd(gb, HL);
  CYC(b_+41, b_+42); E = A;
  TAIL(parseGivenObjectData_b12);
}
