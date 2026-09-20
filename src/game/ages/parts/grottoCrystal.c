#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode24_hook(GB *gb) {
  BASE(partCode24);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+5); A = mem_rd(gb, wSwitchState);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0xc2; // Part.subid
  CYC(b_+8, b_+9); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+12); mem_wr(gb, wSwitchState, A);
  CYC(b_+12, b_+14); L = 0xe4; // Part.collisionType
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+16, b_+18); A = 0x01;
  CALL_C(b_+18, partSetAnimation_hook, SYM(partSetAnimation), b_+21);
  CYC(b_+21, b_+24); SET_BC(0x8280);
  CYC(b_+24, b_+27); objectCreateInteraction_hook(gb); return; // jp

normalStatus:
  CYC(b_+27, b_+29); E = 0xc4; // Part.state
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+33); A = alu_inc8(gb, A);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+37);
  CYC(b_+37, b_+39); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+39, b_+41); goto L_6029; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = 0xe4; // Part.collisionType
  CYC(b_+44, b_+46); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+46, b_+48); A = 0x01;
  CALL_C(b_+48, partSetAnimation_hook, SYM(partSetAnimation), b_+51);

L_6029:
  CALL_C(b_+51, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+54);
  CYC(b_+54, b_+56); H = 0xcf;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x0a);
  CYC(b_+58, b_+61); objectSetVisible83_hook(gb); return; // jp
}
