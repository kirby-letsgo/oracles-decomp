#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode05), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode05), (from), (to), true)

void partCode05_hook(GB *gb);
void switch_updateTile_hook(GB *gb);

void partCode05_hook(GB *gb) {
  BASE(partCode05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);

  CYC(b_+2, b_+5); A = mem_rd(gb, wSwitchState); // wSwitchState
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0xc2; // Part.subid
  CYC(b_+8, b_+9); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+12); mem_wr(gb, wSwitchState, A); // wSwitchState
  CYC(b_+12, b_+15); push_effect(gb, b_+15); switch_updateTile_hook(gb);
  CYC(b_+15, b_+17); A = 0x7e; // SND_SWITCH
  CYC(b_+17, b_+20); playSound_b00_hook(gb); return; // jp

normalStatus:
  CYC(b_+20, b_+22); E = 0xc4; // Part.state
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);

  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+27); L = E;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(b_+28, b_+30); L = 0xcf; // Part.zh
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xfa);
  CALL_C(b_+32, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+35);
  CYC(b_+35, b_+37); E = 0xf0; // Part.var30
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  RET(b_+38); return; // ret
}

void switch_updateTile_hook(GB *gb) {
  BASE(partCode05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+39, b_+41); L = 0xf0; // Part.var30
  CYC(b_+41, b_+42); C = mem_rd(gb, HL);
  CYC(b_+42, b_+45); A = mem_rd(gb, wActiveGroup); // wActiveGroup
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+46, b_+48); goto flipOverworldSwitch; } // jr z
  CYC(b_+46, b_+48);

  CYC(b_+48, b_+51); SET_HL(wSwitchState); // wSwitchState
  CYC(b_+51, b_+53); E = 0xc2; // Part.subid
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+57); A = 0x0a; // TILEINDEX_DUNGEON_SWITCH_OFF
  if (F & FZ) { CYCT(b_+57, b_+59); goto L_453c; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+60); A = alu_inc8(gb, A); // TILEINDEX_DUNGEON_SWITCH_ON

L_453c:
  CYC(b_+60, b_+63); setTile_hook(gb); return; // jp

flipOverworldSwitch:
  CYC(b_+63, b_+65); A = 0x9f; // TILEINDEX_OVERWORLD_SWITCH_ON
  CALL_C(b_+65, setTile_hook, SYM(setTile), b_+68);
  CYC(b_+68, b_+70); B = 0xcf; // >wRoomLayout
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+72); mem_wr(gb, BC, A);
  CALL_C(b_+72, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+77, SYM(partCode06)); partDelete_hook(gb); return; // jp
}
