#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void interactionCodeb1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6771, 0x6774); SET_HL(0x7b60);
  CYC(0x6774, 0x6776); E = 0x3f;
  CYC(0x6776, 0x6779); interBankCall_hook(gb); return;
}

void interactionCodeb2__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x67b5, 0x67b6); A = alu_inc8(gb, A);
  CYC(0x67b6, 0x67b8); mem_wr(gb, DE, A);
  CYC(0x67b7, 0x67bb); mem_wr(gb, 0xcc94, A);
  CYC(0x67ba, 0x67bd); SET_HL(0x67ee);
  CYC(0x67bd, 0x67c1); interactionSetMiniScript_hook(gb); return;
}

void interactionCodeb2__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x677e, 0x6781); A = mem_rd(gb, 0xcc00);
  CYC(0x6781, 0x6783); alu_and(gb, 0x0f);
  CYC(0x6783, 0x6785); A = 0xb3;
  if (F & FZ) CALL_C_CC(0x6785, playSound_b00_hook, 0x0c98, 0x6788); else CYC(0x6785, 0x6788);
  CYC(0x6788, 0x678b); A = mem_rd(gb, 0xcd18);
  CYC(0x678b, 0x678c); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x678c, 0x678e); goto update; }
  CYC(0x678c, 0x678e); A = mem_rd(gb, 0xcd19);
  CYC(0x6791, 0x6792); alu_or(gb, A);
  if (F & FZ) CALL_ROM_CC(0x6792, 0x67c0); else CYC(0x6792, 0x6795);
update:
  CALL_C(0x6795, interactionDecCounter1_hook, 0x23cc, 0x6798);
  if (!(F & FZ)) { CYCT(0x6798, 0x6799); ret_effect(gb); return; }
  CALL_ROM(0x6799, 0x67e1);
  CYC(0x679c, 0x679e); C = 0x0f;
  CALL_C(0x679e, getRandomNumber_hook, 0x043e, 0x67a1);
  CYC(0x67a1, 0x67a2); alu_and(gb, C);
  CYC(0x67a2, 0x67a4); C = alu_srl(gb, C);
  CYC(0x67a4, 0x67a5); C = alu_inc8(gb, C);
  CYC(0x67a5, 0x67a6); alu_sub(gb, C);
  CYC(0x67a6, 0x67a7); C = A;
  CALL_C(0x67a7, getFreePartSlot_hook, 0x3e8e, 0x67aa);
  if (!(F & FZ)) { CYCT(0x67aa, 0x67ab); ret_effect(gb); return; }
  CYC(0x67ab, 0x67ae); mem_wr(gb, HL, 0x11);
  CYC(0x67ad, 0x67ae); L = alu_inc8(gb, L);
  CYC(0x67ae, 0x67b1); mem_wr(gb, HL, 1);
  CYC(0x67b0, 0x67b2); B = 0;
  CYC(0x67b2, 0x67b5); objectCopyPositionWithOffset_hook(gb); return;
}

void interactionCodeb2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6779, checkInteractionState_hook, 0x23fe, 0x677c);
  if (F & FZ) { CYCT(0x677c, 0x677e); interactionCodeb2__state0_hook(gb); return; }
  CYC(0x677c, 0x677e); interactionCodeb2__state1_hook(gb);
}
