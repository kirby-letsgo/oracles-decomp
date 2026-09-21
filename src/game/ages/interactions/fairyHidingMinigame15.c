#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void fairyHidingMinigame_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

void fairyHidingMinigame_spawnForestFairyIndex_hook(GB *gb) {
  BASE(fairyHidingMinigame_spawnForestFairyIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+4);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x49);
  CYC(b_+7, b_+9); L = 0x43;
  CYC(b_+9, b_+10); mem_wr(gb, HL, B);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void fairyHidingMinigame_showFairyFoundText_hook(GB *gb) {
  BASE(fairyHidingMinigame_showFairyFoundText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+3, b_+6); SET_BC(0x0003);
L_6caf:
  CYC(b_+6, b_+7); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_6cb3; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
L_6cb3:
  CYC(b_+10, b_+11); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_6caf; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+17); SET_HL(b_+24);
  CYC(b_+17, b_+18); fairyHidingMinigame_addAToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CYC(b_+19, b_+21); B = 0x11;
  CYC(b_+21, b_+24); TAIL(showText);
}

void fairyHidingMinigame_moveLinkBackLeft_hook(GB *gb) {
  BASE(fairyHidingMinigame_moveLinkBackLeft);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x03);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x18);
  CYC(b_+8, b_+10); A = 0x0b;
  CYC(b_+10, b_+13); mem_wr(gb, wLinkForceState, A);
  CYC(b_+13, b_+15); A = 0x08;
  CYC(b_+15, b_+18); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}
