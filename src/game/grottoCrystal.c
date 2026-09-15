#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode24_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5ff6, 0x5ff8); goto normalStatus; } // jr z
  CYC(0x5ff6, 0x5ff8);
  CYC(0x5ff8, 0x5ffb); A = mem_rd(gb, wSwitchState);
  CYC(0x5ffb, 0x5ffc); H = D;
  CYC(0x5ffc, 0x5ffe); L = 0xc2; // Part.subid
  CYC(0x5ffe, 0x5fff); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x5fff, 0x6002); mem_wr(gb, wSwitchState, A);
  CYC(0x6002, 0x6004); L = 0xe4; // Part.collisionType
  CYC(0x6004, 0x6006); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6006, 0x6008); A = 0x01;
  CALL_C(0x6008, partSetAnimation_hook, 0x2988, 0x600b);
  CYC(0x600b, 0x600e); SET_BC(0x8280);
  CYC(0x600e, 0x6011); objectCreateInteraction_hook(gb); return; // jp

normalStatus:
  CYC(0x6011, 0x6013); E = 0xc4; // Part.state
  CYC(0x6013, 0x6014); A = mem_rd(gb, DE);
  CYC(0x6014, 0x6015); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6015); return; } // ret nz
  CYC(0x6015, 0x6016);
  CYC(0x6016, 0x6017); A = alu_inc8(gb, A);
  CYC(0x6017, 0x6018); mem_wr(gb, DE, A);
  CALL_C(0x6018, getThisRoomFlags_hook, 0x197d, 0x601b);
  CYC(0x601b, 0x601d); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x601d, 0x601f); goto L_6029; } // jr z
  CYC(0x601d, 0x601f);
  CYC(0x601f, 0x6020); H = D;
  CYC(0x6020, 0x6022); L = 0xe4; // Part.collisionType
  CYC(0x6022, 0x6024); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6024, 0x6026); A = 0x01;
  CALL_C(0x6026, partSetAnimation_hook, 0x2988, 0x6029);

L_6029:
  CALL_C(0x6029, objectMakeTileSolid_hook, 0x20b2, 0x602c);
  CYC(0x602c, 0x602e); H = 0xcf;
  CYC(0x602e, 0x6030); mem_wr(gb, HL, 0x0a);
  CYC(0x6030, 0x6033); objectSetVisible83_hook(gb); return; // jp
}
