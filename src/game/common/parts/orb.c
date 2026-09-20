#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode03_hook(GB *gb) {
  BASE(partCode03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0x01); // PARTSTATUS_JUST_HIT
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto notJustHit; } // jr nz
  CYC(b_+2, b_+4);

  CYC(b_+4, b_+7); A = mem_rd(gb, wToggleBlocksState); // wToggleBlocksState
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = 0xc3; // Part.var03
  CYC(b_+10, b_+11); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+14); mem_wr(gb, wToggleBlocksState, A); // wToggleBlocksState
  CYC(b_+14, b_+16); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_and(gb, 0x01);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); A = 0x7e; // SND_SWITCH
  CYC(b_+24, b_+27); playSound_b00_hook(gb); return; // jp

notJustHit:
  CYC(b_+27, b_+29); E = 0xc4; // Part.state
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);

  CYC(b_+32, b_+33); A = alu_inc8(gb, A);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+37);
  CYC(b_+37, b_+39); H = 0xcf; // Part.zh
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0a);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = 0xc2; // Part.subid
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+47); alu_and(gb, 0x07);
  CYC(b_+47, b_+50); SET_BC(0x00f8); // bitTable
  CYC(b_+50, b_+51); alu_add(gb, C);
  CYC(b_+51, b_+52); C = A;
  CYC(b_+52, b_+53); A = mem_rd(gb, BC);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); // [var03]
  CYC(b_+54, b_+57); A = mem_rd(gb, wToggleBlocksState); // wToggleBlocksState
  CYC(b_+57, b_+58); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+58, b_+60); A = 0x01;
  if (F & FZ) { CYCT(b_+60, b_+62); goto L_44c5; } // jr z
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); A = alu_inc8(gb, A);

L_44c5:
  CYC(b_+63, b_+65); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+65, b_+66); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CYC(b_+67, b_+70); objectSetVisible82_hook(gb); return; // jp
}
