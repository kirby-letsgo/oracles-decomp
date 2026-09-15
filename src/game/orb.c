#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4486, 0x4488); alu_cp(gb, 0x01); // PARTSTATUS_JUST_HIT
  if (!(F & FZ)) { CYCT(0x4488, 0x448a); goto notJustHit; } // jr nz
  CYC(0x4488, 0x448a);

  CYC(0x448a, 0x448d); A = mem_rd(gb, 0xcdd2); // wToggleBlocksState
  CYC(0x448d, 0x448e); H = D;
  CYC(0x448e, 0x4490); L = 0xc3; // Part.var03
  CYC(0x4490, 0x4491); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x4491, 0x4494); mem_wr(gb, 0xcdd2, A); // wToggleBlocksState
  CYC(0x4494, 0x4496); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x4496, 0x4497); A = mem_rd(gb, HL);
  CYC(0x4497, 0x4499); alu_and(gb, 0x01);
  CYC(0x4499, 0x449a); A = alu_inc8(gb, A);
  CYC(0x449a, 0x449b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x449b, 0x449c); mem_wr(gb, HL, A);
  CYC(0x449c, 0x449e); A = 0x7e; // SND_SWITCH
  CYC(0x449e, 0x44a1); playSound_b00_hook(gb); return; // jp

notJustHit:
  CYC(0x44a1, 0x44a3); E = 0xc4; // Part.state
  CYC(0x44a3, 0x44a4); A = mem_rd(gb, DE);
  CYC(0x44a4, 0x44a5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x44a5); return; } // ret nz
  CYC(0x44a5, 0x44a6);

  CYC(0x44a6, 0x44a7); A = alu_inc8(gb, A);
  CYC(0x44a7, 0x44a8); mem_wr(gb, DE, A);
  CALL_C(0x44a8, objectMakeTileSolid_hook, 0x20b2, 0x44ab);
  CYC(0x44ab, 0x44ad); H = 0xcf; // Part.zh
  CYC(0x44ad, 0x44af); mem_wr(gb, HL, 0x0a);
  CYC(0x44af, 0x44b0); H = D;
  CYC(0x44b0, 0x44b2); L = 0xc2; // Part.subid
  CYC(0x44b2, 0x44b3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x44b3, 0x44b5); alu_and(gb, 0x07);
  CYC(0x44b5, 0x44b8); SET_BC(0x00f8); // bitTable
  CYC(0x44b8, 0x44b9); alu_add(gb, C);
  CYC(0x44b9, 0x44ba); C = A;
  CYC(0x44ba, 0x44bb); A = mem_rd(gb, BC);
  CYC(0x44bb, 0x44bc); mem_wr(gb, HL, A); // [var03]
  CYC(0x44bc, 0x44bf); A = mem_rd(gb, 0xcdd2); // wToggleBlocksState
  CYC(0x44bf, 0x44c0); alu_and(gb, mem_rd(gb, HL));
  CYC(0x44c0, 0x44c2); A = 0x01;
  if (F & FZ) { CYCT(0x44c2, 0x44c4); goto L_44c5; } // jr z
  CYC(0x44c2, 0x44c4);
  CYC(0x44c4, 0x44c5); A = alu_inc8(gb, A);

L_44c5:
  CYC(0x44c5, 0x44c7); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x44c7, 0x44c8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44c8, 0x44c9); mem_wr(gb, HL, A);
  CYC(0x44c9, 0x44cc); objectSetVisible82_hook(gb); return; // jp
}
