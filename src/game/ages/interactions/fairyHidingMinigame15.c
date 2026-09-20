#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

static void fairyHidingMinigame_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

void fairyHidingMinigame_spawnForestFairyIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c9e, 0x6c9f); B = A;
  CALL_C(0x6c9f, getFreeInteractionSlot_hook, 0x3aef, 0x6ca2);
  if (!(F & FZ)) { CYCT(0x6ca2, 0x6ca3); ret_effect(gb); return; }
  CYC(0x6ca2, 0x6ca3);
  CYC(0x6ca3, 0x6ca5); mem_wr(gb, HL, 0x49);
  CYC(0x6ca5, 0x6ca7); L = 0x43;
  CYC(0x6ca7, 0x6ca8); mem_wr(gb, HL, B);
  CYC(0x6ca8, 0x6ca9); ret_effect(gb);
}

void fairyHidingMinigame_showFairyFoundText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ca9, 0x6cac); A = mem_rd(gb, 0xcfd1);
  CYC(0x6cac, 0x6caf); SET_BC(0x0003);
L_6caf:
  CYC(0x6caf, 0x6cb0); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x6cb0, 0x6cb2); goto L_6cb3; }
  CYC(0x6cb0, 0x6cb2);
  CYC(0x6cb2, 0x6cb3); B = alu_inc8(gb, B);
L_6cb3:
  CYC(0x6cb3, 0x6cb4); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x6cb4, 0x6cb6); goto L_6caf; }
  CYC(0x6cb4, 0x6cb6);
  CYC(0x6cb6, 0x6cb7); A = B;
  CYC(0x6cb7, 0x6cba); SET_HL(0x6cc1);
  CYC(0x6cba, 0x6cbb); fairyHidingMinigame_addAToHl_from_rst(gb, 0x6cbb);
  CYC(0x6cbb, 0x6cbc); C = mem_rd(gb, HL);
  CYC(0x6cbc, 0x6cbe); B = 0x11;
  CYC(0x6cbe, 0x6cc1); showText_hook(gb);
}

void fairyHidingMinigame_moveLinkBackLeft_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6cc4, 0x6cc7); SET_HL(0xd008);
  CYC(0x6cc7, 0x6cc9); mem_wr(gb, HL, 0x03);
  CYC(0x6cc9, 0x6cca); L = alu_inc8(gb, L);
  CYC(0x6cca, 0x6ccc); mem_wr(gb, HL, 0x18);
  CYC(0x6ccc, 0x6cce); A = 0x0b;
  CYC(0x6cce, 0x6cd1); mem_wr(gb, 0xcc4f, A);
  CYC(0x6cd1, 0x6cd3); A = 0x08;
  CYC(0x6cd3, 0x6cd6); mem_wr(gb, 0xcc51, A);
  CYC(0x6cd6, 0x6cd7); ret_effect(gb);
}
