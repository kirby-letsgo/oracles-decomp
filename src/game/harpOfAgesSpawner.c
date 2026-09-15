#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t harp_of_ages_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCodeb3__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6819, getThisRoomFlags_hook, 0x197d, 0x681c);
  CYC(0x681c, 0x681e); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x681e, 0x6821); interactionDelete_hook(gb); return; }
  CYC(0x681e, 0x6821);
  CYC(0x6821, 0x6822); alu_xor(gb, A);
  CYC(0x6822, 0x6825); mem_wr(gb, 0xcfc0, A);
  CALL_C(0x6825, getFreeInteractionSlot_hook, 0x3aef, 0x6828);
  if (!(F & FZ)) { CYCT(0x6828, 0x6829); ret_effect(gb); return; }
  CYC(0x6828, 0x682b); mem_wr(gb, HL, 0x60);
  CYC(0x682b, 0x682c); L = alu_inc8(gb, L);
  CYC(0x682c, 0x682e); mem_wr(gb, HL, 0x11);
  CYC(0x682e, 0x6830); L = 0x4b;
  CYC(0x6830, 0x6833); mem_wr(gb, HL, 0x38);
  CYC(0x6832, 0x6834); L = 0x4d;
  CYC(0x6834, 0x6836); mem_wr(gb, HL, 0x58);
  CYC(0x6836, 0x6837); B = H;
  CALL_C(0x6837, getFreeInteractionSlot_hook, 0x3aef, 0x683a);
  if (!(F & FZ)) { CYCT(0x683a, 0x683c); goto inc_state; }
  CYC(0x683a, 0x683c);
  CYC(0x683c, 0x683e); mem_wr(gb, HL, 0x84);
  CYC(0x683e, 0x683f); L = alu_inc8(gb, L);
  CYC(0x683f, 0x6841); mem_wr(gb, HL, 0x0c);
  CYC(0x6841, 0x6843); L = 0x56;
  CYC(0x6843, 0x6845); A = 0x40;
  CYC(0x6845, 0x6846); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6846, 0x6847); mem_wr(gb, HL, B);
inc_state:
  CALL_C(0x6847, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x684a);
  CYC(0x684a, 0x684d); interactionIncState_hook(gb); return;
}

void interactionCodeb3__incState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6847, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x684a);
  CYC(0x684a, 0x684d); interactionIncState_hook(gb); return;
}

void interactionCodeb3__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x684d, getThisRoomFlags_hook, 0x197d, 0x6850);
  CYC(0x6850, 0x6852); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6852, 0x6853); ret_effect(gb); return; }
  CYC(0x6852, 0x6853);
  CYC(0x6853, 0x6855); A = 0xf0;
  CALL_C(0x6855, playSound_b00_hook, 0x0c98, 0x6858);
  CYC(0x6858, 0x685a); A = 0x80;
  CYC(0x685a, 0x685d); mem_wr(gb, 0xcc8a, A);
  CYC(0x685d, 0x6860); mem_wr(gb, 0xcc02, A);
  CALL_C(0x6860, interactionIncState_hook, 0x23e0, 0x6863);
}

void interactionCodeb3__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6863, 0x6866); A = mem_rd(gb, 0xcba0);
  CYC(0x6866, 0x6867); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6867, 0x6868); ret_effect(gb); return; }
  CYC(0x6867, 0x6868);
  CYC(0x6868, 0x6869); alu_xor(gb, A);
  CYC(0x6869, 0x686c); mem_wr(gb, 0xd008, A);
  CYC(0x686c, 0x686f); interactionIncState_hook(gb); return;
}

void interactionCodeb3__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x686f, 0x6872); A = mem_rd(gb, 0xcba0);
  CYC(0x6872, 0x6873); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6873, 0x6874); ret_effect(gb); return; }
  CYC(0x6873, 0x6874);
  CYC(0x6874, 0x6877); SET_HL(0xcfc0);
  CYC(0x6877, 0x6879); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 1));
  CALL_C(0x6879, interactionIncState_hook, 0x23e0, 0x687c);
  CYC(0x687c, 0x687e); L = 0x46;
  CYC(0x687e, 0x6881); mem_wr(gb, HL, 0x28);
  CYC(0x6880, 0x6882); A = 0x02;
  CALL_C(0x6882, fadeoutToBlackWithDelay_hook, 0x32ab, 0x6885);
  CYC(0x6885, 0x6887); A = 0xff;
  CYC(0x6887, 0x688a); mem_wr(gb, 0xc4b1, A);
  CYC(0x688a, 0x688d); mem_wr(gb, 0xc4b3, A);
  CYC(0x688d, 0x688f); A = 1;
  CYC(0x688f, 0x6892); mem_wr(gb, 0xc4b2, A);
  CYC(0x6892, 0x6894); A = 0xfe;
  CYC(0x6894, 0x6897); mem_wr(gb, 0xc4b4, A);
  CALL_C(0x6897, hideStatusBar_hook, 0x1aa0, 0x689a);
  CYC(0x689a, 0x689c); A = mem_rd(gb, 0xffaf);
  CYC(0x689c, 0x689d); D = A;
  CYC(0x689d, 0x689e); ret_effect(gb);
}

void interactionCodeb3__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x689e, 0x68a1); A = mem_rd(gb, 0xc4ab);
  CYC(0x68a1, 0x68a2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x68a2, 0x68a3); ret_effect(gb); return; }
  CYC(0x68a2, 0x68a3);
  CALL_C(0x68a3, interactionDecCounter1_hook, 0x23cc, 0x68a6);
  if (!(F & FZ)) { CYCT(0x68a6, 0x68a7); ret_effect(gb); return; }
  CYC(0x68a6, 0x68a7);
  CYC(0x68a7, 0x68a8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x68a8, getFreeInteractionSlot_hook, 0x3aef, 0x68ab);
  if (!(F & FZ)) { CYCT(0x68ab, 0x68ac); ret_effect(gb); return; }
  CYC(0x68ab, 0x68ac);
  CYC(0x68ac, 0x68ae); mem_wr(gb, HL, 0x36);
  CYC(0x68ae, 0x68af); L = alu_inc8(gb, L);
  CYC(0x68af, 0x68b1); mem_wr(gb, HL, 0x07);
  CALL_C(0x68b1, objectCopyPosition_hook, 0x2242, 0x68b4);
  CYC(0x68b4, 0x68b7); interactionDelete_hook(gb); return;
}

void interactionCodeb3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x680b, 0x680d); E = 0x44;
  CYC(0x680d, 0x680f); A = mem_rd(gb, DE);
  CYC(0x680e, 0x680f); push_effect(gb, 0x680f);
  switch (harp_of_ages_jump_table(gb)) {
    case 0x6819: interactionCodeb3__state0_hook(gb); return;
    case 0x684d: interactionCodeb3__state1_hook(gb); return;
    case 0x6863: interactionCodeb3__state2_hook(gb); return;
    case 0x686f: interactionCodeb3__state3_hook(gb); return;
    case 0x689e: interactionCodeb3__state4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
