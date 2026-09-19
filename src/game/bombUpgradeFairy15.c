#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void bombUpgradeFairy_linkCollapsed_hook(GB *gb);

void bombUpgradeFairy_spawnBombsAroundLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f9d, 0x6f9f); B = 0x04;
next:
  CALL_C(0x6f9f, getFreeInteractionSlot_hook, 0x3aef, 0x6fa2);
  if (!(F & FZ)) { RET_TAKEN(0x6fa2); return; }
  CYC(0x6fa2, 0x6fa3);
  CYC(0x6fa3, 0x6fa5); mem_wr(gb, HL, 0x83);
  CYC(0x6fa5, 0x6fa6); L = alu_inc8(gb, L);
  CYC(0x6fa6, 0x6fa7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6fa7, 0x6fa8); L = alu_inc8(gb, L);
  CYC(0x6fa8, 0x6fa9); B = alu_dec8(gb, B);
  CYC(0x6fa9, 0x6faa); mem_wr(gb, HL, B);
  if (!(F & FZ)) { CYCT(0x6faa, 0x6fac); goto next; }
  CYC(0x6faa, 0x6fac);
  CYC(0x6fac, 0x6fad); ret_effect(gb);
}

void bombUpgradeFairy_lightningStrikesLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6fad, getFreePartSlot_hook, 0x3e8e, 0x6fb0);
  if (!(F & FZ)) { RET_TAKEN(0x6fb0); return; }
  CYC(0x6fb0, 0x6fb1);
  CYC(0x6fb1, 0x6fb2); L = alu_dec8(gb, L);
  CYC(0x6fb2, 0x6fb4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6fb4, 0x6fb5); L = alu_inc8(gb, L);
  CYC(0x6fb5, 0x6fb7); mem_wr(gb, HL, 0x27);
  CYC(0x6fb7, 0x6fb8); L = alu_inc8(gb, L);
  CYC(0x6fb8, 0x6fb9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6fb9, 0x6fbb); L = 0xcb;
  CYC(0x6fbb, 0x6fbd); A = hram_rd(gb, 0xb0);
  CYC(0x6fbd, 0x6fbe); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6fbe, 0x6fbf); L = alu_inc8(gb, L);
  CYC(0x6fbf, 0x6fc1); A = hram_rd(gb, 0xb1);
  CYC(0x6fc1, 0x6fc2); mem_wr(gb, HL, A);
  CYC(0x6fc2, 0x6fc3); ret_effect(gb);
}

void bombUpgradeFairy_decreaseLinkHealth_hook(GB *gb) {
  CYC(0x6fc3, 0x6fc6); SET_HL(wLinkHealth);
  CYC(0x6fc6, 0x6fc7); A = mem_rd(gb, HL);
  CYC(0x6fc7, 0x6fc9); alu_cp(gb, 0x04);
  if (F & FC) { RET_TAKEN(0x6fc9); return; }
  CYC(0x6fc9, 0x6fca);
  CYC(0x6fca, 0x6fcc); mem_wr(gb, HL, 0x04);
  bombUpgradeFairy_linkCollapsed_hook(gb);
}

void bombUpgradeFairy_linkCollapsed_hook(GB *gb) {
  CYC(0x6fcc, 0x6fce); A = 0x02;
  CYC(0x6fce, 0x6fd1); mem_wr(gb, wcc50, A);
  CYC(0x6fd1, 0x6fd2); ret_effect(gb);
}

void bombUpgradeFairy_loseAllBombs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6fd2, 0x6fd4); A = 0x01;
  CYC(0x6fd4, 0x6fd7); mem_wr(gb, wNumBombs, A);
  CALL_C(0x6fd7, decNumBombs_hook, 0x17c8, 0x6fda);
  CYC(0x6fda, 0x6fdc); bombUpgradeFairy_linkCollapsed_hook(gb);
}

void bombUpgradeFairy_giveBombUpgrade_hook(GB *gb) {
  CYC(0x6fdc, 0x6fdf); A = mem_rd(gb, wTextNumberSubstitution);
  CYC(0x6fdf, 0x6fe2); mem_wr(gb, wMaxBombs, A);
  CYC(0x6fe2, 0x6fe3); C = A;
  CYC(0x6fe3, 0x6fe5); A = 0x03;
  CYC(0x6fe5, 0x6fe8); giveTreasure_hook(gb);
}

void bombUpgradeFairy_fadeinFromWhite_hook(GB *gb) {
  CYC(0x6fe8, 0x6fea); A = 0xff;
  CYC(0x6fea, 0x6fed); mem_wr(gb, 0xcfd0, A);
  CYC(0x6fed, 0x6fef); A = 0x04;
  CYC(0x6fef, 0x6ff2); fadeinFromWhiteWithDelay_hook(gb);
}

void bombUpgradeFairy_setGlobalFlag_hook(GB *gb) {
  CYC(0x6ff2, 0x6ff4); A = 0x1c;
  CYC(0x6ff4, 0x6ff7); setGlobalFlag_hook(gb);
}
