#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// PART_TINGLE_BALLOON
void partCode44_hook(GB *gb) {
  BASE(partCode44);
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); goto beenHit; } // jr nz
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xc4; // Part.state
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto state1; } // jr nz
  CYC(b_+6, b_+8);

state0:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+10); L = E;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = 0xc6; // Part.counter1
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x38);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xff); // Part.counter2
  CYC(b_+18, b_+20); L = 0xcf; // Part.zh
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xf1);
  CYC(b_+22, b_+25); SET_BC((hSoundChannelAddresses + 14));
  CALL_C(b_+25, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+28);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  CALL_C(b_+29, partSetAnimation_hook, SYM(partSetAnimation), b_+32);
  CALL_C(b_+32, objectSetVisible81_hook, SYM(objectSetVisible81), b_+35);

state1:
  CALL_C(b_+35, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+38);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto L_7509; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x38); // Part.counter1
  CYC(b_+42, b_+44); L = 0xd4; // Part.speedZ
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); alu_cpl(gb);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+50); alu_cpl(gb);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);

L_7509:
  CYC(b_+51, b_+53); C = 0x00;
  CALL_C(b_+53, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+56);
  CYC(b_+56, b_+58); A = 0x0f; // Object.zh
  CALL_C(b_+58, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+61);
  CYC(b_+61, b_+63); E = 0xcf; // Part.zh
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  RET(b_+65); return; // ret

beenHit:
  CYC(b_+66, b_+68); A = 0x04; // Object.state
  CALL_C(b_+68, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+71);
  CYC(b_+71, b_+72); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+72, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(b_+77, b_+79); L = 0x43; // Interaction.var03
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x01);
  CYC(b_+81, b_+84); SET_BC(0xf000);
  CALL_C(b_+84, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+87);
  CYC(b_+87, b_+90); TAIL(partDelete); // jp
}
