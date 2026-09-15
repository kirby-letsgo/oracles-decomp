#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_TINGLE_BALLOON
void partCode44_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(0x74d6, 0x74d8); goto beenHit; } // jr nz
  CYC(0x74d6, 0x74d8);
  CYC(0x74d8, 0x74da); E = 0xc4; // Part.state
  CYC(0x74da, 0x74db); A = mem_rd(gb, DE);
  CYC(0x74db, 0x74dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74dc, 0x74de); goto state1; } // jr nz
  CYC(0x74dc, 0x74de);

state0:
  CYC(0x74de, 0x74df); H = D;
  CYC(0x74df, 0x74e0); L = E;
  CYC(0x74e0, 0x74e1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x74e1, 0x74e3); L = 0xc6; // Part.counter1
  CYC(0x74e3, 0x74e5); mem_wr(gb, HL, 0x38);
  CYC(0x74e5, 0x74e6); L = alu_inc8(gb, L);
  CYC(0x74e6, 0x74e8); mem_wr(gb, HL, 0xff); // Part.counter2
  CYC(0x74e8, 0x74ea); L = 0xcf; // Part.zh
  CYC(0x74ea, 0x74ec); mem_wr(gb, HL, 0xf1);
  CYC(0x74ec, 0x74ef); SET_BC(0xfff0);
  CALL_C(0x74ef, objectSetSpeedZ_hook, 0x239d, 0x74f2);
  CYC(0x74f2, 0x74f3); A = 0x00; // xor a
  CALL_C(0x74f3, partSetAnimation_hook, 0x2988, 0x74f6);
  CALL_C(0x74f6, objectSetVisible81_hook, 0x1e60, 0x74f9);

state1:
  CALL_C(0x74f9, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x74fc);
  if (!(F & FZ)) { CYCT(0x74fc, 0x74fe); goto L_7509; } // jr nz
  CYC(0x74fc, 0x74fe);
  CYC(0x74fe, 0x7500); mem_wr(gb, HL, 0x38); // Part.counter1
  CYC(0x7500, 0x7502); L = 0xd4; // Part.speedZ
  CYC(0x7502, 0x7503); A = mem_rd(gb, HL);
  CYC(0x7503, 0x7504); alu_cpl(gb);
  CYC(0x7504, 0x7505); A = alu_inc8(gb, A);
  CYC(0x7505, 0x7506); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7506, 0x7507); A = mem_rd(gb, HL);
  CYC(0x7507, 0x7508); alu_cpl(gb);
  CYC(0x7508, 0x7509); mem_wr(gb, HL, A);

L_7509:
  CYC(0x7509, 0x750b); C = 0x00;
  CALL_C(0x750b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x750e);
  CYC(0x750e, 0x7510); A = 0x0f; // Object.zh
  CALL_C(0x7510, objectGetRelatedObject1Var_hook, 0x2160, 0x7513);
  CYC(0x7513, 0x7515); E = 0xcf; // Part.zh
  CYC(0x7515, 0x7516); A = mem_rd(gb, DE);
  CYC(0x7516, 0x7517); mem_wr(gb, HL, A);
  RET(0x7517); return; // ret

beenHit:
  CYC(0x7518, 0x751a); A = 0x04; // Object.state
  CALL_C(0x751a, objectGetRelatedObject1Var_hook, 0x2160, 0x751d);
  CYC(0x751d, 0x751e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x751e, getFreeInteractionSlot_hook, 0x3aef, 0x7521);
  CYC(0x7521, 0x7523); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(0x7523, 0x7525); L = 0x43; // Interaction.var03
  CYC(0x7525, 0x7527); mem_wr(gb, HL, 0x01);
  CYC(0x7527, 0x752a); SET_BC(0xf000);
  CALL_C(0x752a, objectCopyPositionWithOffset_hook, 0x225a, 0x752d);
  CYC(0x752d, 0x7530); partDelete_hook(gb); return; // jp
}
