#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode05_hook(GB *gb);
void switch_updateTile_hook(GB *gb);

void partCode05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4500, 0x4502); goto normalStatus; } // jr z
  CYC(0x4500, 0x4502);

  CYC(0x4502, 0x4505); A = mem_rd(gb, 0xcdd3); // wSwitchState
  CYC(0x4505, 0x4506); H = D;
  CYC(0x4506, 0x4508); L = 0xc2; // Part.subid
  CYC(0x4508, 0x4509); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x4509, 0x450c); mem_wr(gb, 0xcdd3, A); // wSwitchState
  CYC(0x450c, 0x450f); push_effect(gb, 0x450f); switch_updateTile_hook(gb);
  CYC(0x450f, 0x4511); A = 0x7e; // SND_SWITCH
  CYC(0x4511, 0x4514); playSound_b00_hook(gb); return; // jp

normalStatus:
  CYC(0x4514, 0x4516); E = 0xc4; // Part.state
  CYC(0x4516, 0x4517); A = mem_rd(gb, DE);
  CYC(0x4517, 0x4518); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4518); return; } // ret nz
  CYC(0x4518, 0x4519);

  CYC(0x4519, 0x451a); H = D;
  CYC(0x451a, 0x451b); L = E;
  CYC(0x451b, 0x451c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(0x451c, 0x451e); L = 0xcf; // Part.zh
  CYC(0x451e, 0x4520); mem_wr(gb, HL, 0xfa);
  CALL_C(0x4520, objectGetShortPosition_hook, 0x2096, 0x4523);
  CYC(0x4523, 0x4525); E = 0xf0; // Part.var30
  CYC(0x4525, 0x4526); mem_wr(gb, DE, A);
  RET(0x4526); return; // ret
}

void switch_updateTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4527, 0x4529); L = 0xf0; // Part.var30
  CYC(0x4529, 0x452a); C = mem_rd(gb, HL);
  CYC(0x452a, 0x452d); A = mem_rd(gb, 0xcc2d); // wActiveGroup
  CYC(0x452d, 0x452e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x452e, 0x4530); goto flipOverworldSwitch; } // jr z
  CYC(0x452e, 0x4530);

  CYC(0x4530, 0x4533); SET_HL(0xcdd3); // wSwitchState
  CYC(0x4533, 0x4535); E = 0xc2; // Part.subid
  CYC(0x4535, 0x4536); A = mem_rd(gb, DE);
  CYC(0x4536, 0x4537); alu_and(gb, mem_rd(gb, HL));
  CYC(0x4537, 0x4539); A = 0x0a; // TILEINDEX_DUNGEON_SWITCH_OFF
  if (F & FZ) { CYCT(0x4539, 0x453b); goto L_453c; } // jr z
  CYC(0x4539, 0x453b);
  CYC(0x453b, 0x453c); A = alu_inc8(gb, A); // TILEINDEX_DUNGEON_SWITCH_ON

L_453c:
  CYC(0x453c, 0x453f); setTile_hook(gb); return; // jp

flipOverworldSwitch:
  CYC(0x453f, 0x4541); A = 0x9f; // TILEINDEX_OVERWORLD_SWITCH_ON
  CALL_C(0x4541, setTile_hook, 0x3a9c, 0x4544);
  CYC(0x4544, 0x4546); B = 0xcf; // >wRoomLayout
  CYC(0x4546, 0x4547); alu_xor(gb, A);
  CYC(0x4547, 0x4548); mem_wr(gb, BC, A);
  CALL_C(0x4548, getThisRoomFlags_hook, 0x197d, 0x454b);
  CYC(0x454b, 0x454d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x454d, 0x4550); partDelete_hook(gb); return; // jp
}
